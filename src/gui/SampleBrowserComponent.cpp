#include "../../include/undergroundBeats/gui/SampleBrowserComponent.h"
#include "JuceHeader.h"

//==============================================================================
SampleBrowserComponent::SampleBrowserComponent()
    // fileFilter is default-constructed (now uses AudioFileFilter)
{
    DBG("SampleBrowserComponent: Initializing...");
    formatManager.registerBasicFormats();
    
    // Use the simple FileBrowserComponent constructor with explicit cast
    fileBrowser.reset(new juce::FileBrowserComponent(
        juce::FileBrowserComponent::openMode | 
        juce::FileBrowserComponent::canSelectFiles |
        juce::FileBrowserComponent::filenameBoxIsReadOnly,
        juce::File::getSpecialLocation(juce::File::userHomeDirectory), // Initial directory
        &fileFilter, // Pass our custom AudioFileFilter instance
        (juce::FilePreviewComponent*) nullptr)); // Explicit cast for preview component
    
    addAndMakeVisible(fileBrowser.get());
    fileBrowser->addListener(this);
    
    addAndMakeVisible(playButton);
    playButton.addListener(this);
    
    deviceManager.initialiseWithDefaultDevices(0, 2);
    deviceManager.addAudioCallback(&audioSourcePlayer);
    audioSourcePlayer.setSource(&transportSource);
    
    setSize(400, 600);
    DBG("SampleBrowserComponent: Initialized.");
}

SampleBrowserComponent::~SampleBrowserComponent()
{
    DBG("SampleBrowserComponent: Destroying...");
    deviceManager.removeAudioCallback(&audioSourcePlayer);
    audioSourcePlayer.setSource(nullptr);
    transportSource.setSource(nullptr);
    DBG("SampleBrowserComponent: Destroyed.");
}

void SampleBrowserComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds(), 1);
    
    // Draw drag highlight if active (NEW)
    if (isShowingDragHighlight)
    {
        g.setColour(juce::Colours::lightblue.withAlpha(0.3f));
        g.fillRect(getLocalBounds());
        g.setColour(juce::Colours::lightblue);
        g.drawRect(getLocalBounds(), 2);
        g.setColour(juce::Colours::white);
        g.setFont(16.0f);
        g.drawText("Drop audio file", getLocalBounds(), juce::Justification::centred, true);
    }
}

void SampleBrowserComponent::resized()
{
    auto bounds = getLocalBounds().reduced(2);
    
    // Reserve space for the play button at the bottom
    auto buttonArea = bounds.removeFromBottom(30);
    playButton.setBounds(buttonArea.reduced(2));
    
    // Use the rest of the space for the file browser
    fileBrowser->setBounds(bounds);
}

//==============================================================================
// FileBrowserListener Callbacks
//==============================================================================
void SampleBrowserComponent::selectionChanged()
{
    DBG("SampleBrowserComponent: selectionChanged");
    // This is called when the user selects a file in the browser
    stopPreview();
    
    if (fileBrowser->getNumSelectedFiles() > 0)
    {
        selectedFile = fileBrowser->getSelectedFile(0);
        DBG("Selected file: " + selectedFile.getFullPathName());
    }
    else
    {
        DBG("Selection cleared.");
        selectedFile = juce::File(); // Clear selection
    }
}

void SampleBrowserComponent::fileClicked(const juce::File& file, const juce::MouseEvent& e)
{
    DBG("SampleBrowserComponent: fileClicked - " + file.getFileName() + ", Clicks: " + juce::String(e.getNumberOfClicks()));
    // This is called when the user clicks on a file in the browser
    selectedFile = file;
    
    if (e.getNumberOfClicks() == 2)
    {
        startPreview(file);
    }
}

void SampleBrowserComponent::fileDoubleClicked(const juce::File& file)
{
    DBG("SampleBrowserComponent: fileDoubleClicked - " + file.getFileName());
    // This is called when the user double-clicks on a file in the browser
    selectedFile = file;
    startPreview(file);
    
    // Notify listeners if a callback is set
    if (onSampleDropped)
    {
        DBG("Calling onSampleDropped callback.");
        onSampleDropped(file);
    }
}

void SampleBrowserComponent::browserRootChanged(const juce::File& newRoot)
{
    DBG("SampleBrowserComponent: browserRootChanged - " + newRoot.getFullPathName());
    // This is called when the user navigates to a different directory
    // directoryList is managed by FileBrowserComponent, no need to setDirectory here
}

//==============================================================================
// Button Listener Callback
//==============================================================================
void SampleBrowserComponent::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        if (transportSource.isPlaying())
        {
            stopPreview();
            playButton.setButtonText("Play");
        }
        else if (selectedFile.existsAsFile())
        {
            startPreview(selectedFile);
            playButton.setButtonText("Stop");
        }
    }
}

//==============================================================================
// FileDragAndDropTarget Callbacks
//==============================================================================
bool SampleBrowserComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& filePath : files)
    {
        juce::File file(filePath);
        
        // Rely solely on our custom filter now
        if (fileFilter.isFileSuitable(file))
        {
            return true;
        }
    }
    return false;
}

void SampleBrowserComponent::filesDropped(const juce::StringArray& files, int x, int y)
{
    DBG("SampleBrowserComponent: filesDropped");
    isShowingDragHighlight = false;
    repaint();

    for (const auto& file : files)
    {
        juce::File droppedFile(file);
        // Rely solely on our custom filter now
        if (fileFilter.isFileSuitable(droppedFile)) 
        {
            DBG("Suitable file dropped: " + droppedFile.getFullPathName());
            if (onSampleDropped)
            {
                 onSampleDropped(droppedFile);
            }
            else
            {
                DBG("No external callback. Selecting and previewing dropped file.");
                selectedFile = droppedFile;
                startPreview(selectedFile);
            }
            break; 
        }
    }
}

void SampleBrowserComponent::fileDragEnter(const juce::StringArray& files, int x, int y)
{
    DBG("SampleBrowserComponent: fileDragEnter");
    if (isInterestedInFileDrag(files))
    {
        isShowingDragHighlight = true;
        repaint();
    }
}

void SampleBrowserComponent::fileDragExit(const juce::StringArray& files)
{
    DBG("SampleBrowserComponent: fileDragExit");
    isShowingDragHighlight = false;
    repaint();
}

//==============================================================================
// Preview Helpers
//==============================================================================
void SampleBrowserComponent::startPreview(const juce::File& file)
{
    DBG("SampleBrowserComponent: startPreview - " + file.getFileName());
    // Stop any existing preview
    stopPreview();
    
    // Check if the file is a supported audio format
    auto* reader = formatManager.createReaderFor(file);
    
    if (reader != nullptr)
    {
        DBG("Reader created successfully.");
        // Create a source that will read from the file
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        
        // Prepare the transport source with our reader
        transportSource.setSource(readerSource.get());
        transportSource.setPosition(0.0);
        transportSource.start();
        DBG("Transport source started.");
        
        playButton.setButtonText("Stop");
    }
    else
    {
        DBG("Failed to create reader for preview.");
    }
}

void SampleBrowserComponent::stopPreview()
{
    if (transportSource.isPlaying() || readerSource != nullptr)
    {
        DBG("SampleBrowserComponent: stopPreview");
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();
        playButton.setButtonText("Play");
    }
}