#include "undergroundBeats/gui/MainEditor.h"
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace undergroundBeats {
namespace gui {

MainEditor::MainEditor(juce::AudioProcessor& processor, core::UndergroundBeatsController& controller)
    : juce::AudioProcessorEditor(processor),
      controller(controller),
      formatManager(std::make_unique<juce::AudioFormatManager>()),
      thumbnailCache(std::make_unique<juce::AudioThumbnailCache>(100)) {
    
    // Initialize format manager
    formatManager->registerBasicFormats();
    
    // Set the UI size
    setSize(1200, 800);
    
    // Initialize UI components
    initializeUI();
    
    // Apply dark theme to all components
    applyDarkTheme();
    
    // Register for controller updates
    controller.registerUIUpdateCallback([this]() {
        // Run on the message thread
        juce::MessageManager::callAsync([this]() {
            updateUI();
        });
    });
}

MainEditor::~MainEditor() = default;

void MainEditor::paint(juce::Graphics& g) {
    // Fill background with dark color
    g.fillAll(backgroundColour);
    
    // Add subtle gradient for depth
    juce::ColourGradient gradient(
        backgroundColour.brighter(0.1f), 0.0f, 0.0f,
        backgroundColour.darker(0.1f), 0.0f, static_cast<float>(getHeight()),
        false);
    
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());
    
    // Draw accent line at the top
    g.setColour(accentColour);
    g.fillRect(0, 0, getWidth(), 2);
}

void MainEditor::resized() {
    // Define layout constants
    const int margin = 10;
    const int topBarHeight = 40;
    const int bottomPanelHeight = 60;
    const int leftSidebarWidth = 200;
    const int effectIconsHeight = 50;
    
    // Layout the main UI sections
    juce::Rectangle<int> area = getLocalBounds();
    
    // Top bar
    juce::Rectangle<int> topBar = area.removeFromTop(topBarHeight);
    topBar.reduce(margin, 0);
    
    // Place load and save buttons in the top bar
    loadButton.setBounds(topBar.removeFromLeft(100));
    topBar.removeFromLeft(margin);
    saveButton.setBounds(topBar.removeFromLeft(100));
    topBar.removeFromLeft(margin);
    generationMethodComboBox.setBounds(topBar.removeFromLeft(150));
    
    // Bottom panel with transport controls
    juce::Rectangle<int> bottomPanel = area.removeFromBottom(bottomPanelHeight);
    bottomPanel.reduce(margin, margin);
    
    // Place transport controls in the bottom panel
    if (playButton) playButton->setBounds(bottomPanel.removeFromLeft(50));
    bottomPanel.removeFromLeft(margin);
    if (stopButton) stopButton->setBounds(bottomPanel.removeFromLeft(50));
    bottomPanel.removeFromLeft(margin);
    if (recordButton) recordButton->setBounds(bottomPanel.removeFromLeft(50));
    bottomPanel.removeFromLeft(margin);
    if (timelineSlider) timelineSlider->setBounds(bottomPanel);
    
    // Left sidebar with file tree
    juce::Rectangle<int> leftSidebar = area.removeFromLeft(leftSidebarWidth);
    leftSidebar.reduce(margin, margin);
    
    if (fileTree) fileTree->setBounds(leftSidebar);
    
    // Effect icons row at the top of the main workspace
    juce::Rectangle<int> effectIconsRow = area.removeFromTop(effectIconsHeight);
    effectIconsRow.reduce(margin, margin);
    
    // Place effect icons
    const int effectIconWidth = 40;
    const int effectSpacing = 10;
    int x = effectIconsRow.getX();
    
    for (auto& button : effectButtons) {
        button->setBounds(x, effectIconsRow.getY(), effectIconWidth, effectIconsHeight);
        x += effectIconWidth + effectSpacing;
    }
    
    // Main workspace with waveforms
    juce::Rectangle<int> workspace = area.reduced(margin, margin);
    
    if (waveformViewport) {
        waveformViewport->setBounds(workspace);
        
        if (waveformContainer) {
            // Make container at least as tall as viewport
            waveformContainer->setSize(
                workspace.getWidth() - 20,  // Account for scrollbar
                std::max(workspace.getHeight(), 
                         50 * static_cast<int>(componentUIs.size())));
            
            // Layout component waveforms and controls
            const int componentHeight = 50;
            const int componentSpacing = 10;
            int y = 0;
            
            for (auto& [name, ui] : componentUIs) {
                // Waveform area (left side)
                juce::Rectangle<int> componentArea(
                    0, y, 
                    waveformContainer->getWidth() * 2 / 3, componentHeight);
                
                if (ui.thumbnail) {
                    juce::Rectangle<int> thumbnailArea = componentArea.reduced(5, 5);
                    // AudioThumbnail doesn't have setBounds, it's drawn in a component
                    ui.thumbnailComponent->setBounds(thumbnailArea);
                }
                
                // Controls area (right side)
                juce::Rectangle<int> controlsArea(
                    waveformContainer->getWidth() * 2 / 3, y, 
                    waveformContainer->getWidth() / 3, componentHeight);
                
                // Place controls
                if (ui.gainSlider) ui.gainSlider->setBounds(controlsArea.removeFromLeft(80));
                if (ui.pitchSlider) ui.pitchSlider->setBounds(controlsArea.removeFromLeft(80));
                if (ui.tempoSlider) ui.tempoSlider->setBounds(controlsArea.removeFromLeft(80));
                if (ui.effectsComboBox) ui.effectsComboBox->setBounds(controlsArea.removeFromLeft(100));
                if (ui.soloButton) ui.soloButton->setBounds(controlsArea.removeFromLeft(50));
                if (ui.muteButton) ui.muteButton->setBounds(controlsArea.removeFromLeft(50));
                if (ui.generateVariationsButton) ui.generateVariationsButton->setBounds(controlsArea.removeFromLeft(150));
                
                // Move to next component
                y += componentHeight + componentSpacing;
            }
        }
    }
    
    // Variation explorer (if visible)
    if (variationExplorer && variationExplorer->isVisible()) {
        variationExplorer->setBounds(getLocalBounds().reduced(100, 100));
    }
}

void MainEditor::initializeUI() {
    // Initialize top bar buttons
    loadButton.setButtonText("Load");
    loadButton.onClick = [this]() { handleLoadSample(); };
    addAndMakeVisible(loadButton);
    
    saveButton.setButtonText("Save");
    saveButton.onClick = [this]() { handleSaveOutput(); };
    addAndMakeVisible(saveButton);
    
    generationMethodComboBox.addItem("Algorithmic", 1);
    generationMethodComboBox.addItem("GAN", 2);
    generationMethodComboBox.addItem("VAE", 3);
    generationMethodComboBox.addItem("Style Transfer", 4);
    generationMethodComboBox.setSelectedId(1);
    addAndMakeVisible(generationMethodComboBox);
    
    // Initialize main workspace
    waveformViewport = std::make_unique<juce::Viewport>();
    waveformViewport->setScrollBarsShown(true, false);
    addAndMakeVisible(*waveformViewport);
    
    waveformContainer = std::make_unique<juce::Component>();
    waveformViewport->setViewedComponent(waveformContainer.get(), false);
    waveformContainer->setVisible(true);
    
    // Initialize left sidebar with file tree
    audioFileFilter = std::make_unique<juce::WildcardFileFilter>("*.wav;*.mp3;*.aif", "", "Audio Files");
    timeSliceThread = std::make_unique<juce::TimeSliceThread>("File Scanner Thread");
    timeSliceThread->startThread();
    directoryContentsList = std::make_unique<juce::DirectoryContentsList>(audioFileFilter.get(), *timeSliceThread);
    directoryContentsList->setDirectory(juce::File::getSpecialLocation(juce::File::userHomeDirectory), true, true);
    
    fileTree = std::make_unique<juce::FileTreeComponent>(*directoryContentsList);
    addAndMakeVisible(*fileTree);
    
    // Initialize transport controls
    createTransportControls();
    
    // Initialize effect buttons
    createEffectButtons();
    
    // Initialize variation explorer
    createVariationExplorerUI();
}

void MainEditor::updateUI() {
    // Update component UIs based on current controller state
    
    // Get current component names
    std::vector<std::string> componentNames = controller.getComponentNames();
    
    // Create UIs for new components
    juce::Colour componentColours[] = {
        bassColour,
        drumsColour,
        midColour,
        instrumentsColour
    };
    
    int colourIndex = 0;
    for (const auto& name : componentNames) {
        if (componentUIs.find(name) == componentUIs.end()) {
            // Create UI for this component
            createComponentUI(name, componentColours[colourIndex % 4]);
            colourIndex++;
        }
    }
    
    // Layout the components
    resized();
    
    // Repaint
    repaint();
}

void MainEditor::handleLoadSample() {
    juce::FileChooser chooser("Select an audio file to load",
                               juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                               "*.wav;*.mp3;*.aif");
    
    chooser.launchAsync(juce::FileBrowserComponent::openMode, [this](const juce::FileChooser& fileChooser) {
        if (fileChooser.getResults().size() > 0) {
            juce::File file = fileChooser.getResult();
            controller.loadAudioFile(file.getFullPathName().toStdString());
        }
    });
}

void MainEditor::handleSaveOutput() {
    juce::FileChooser chooser("Save audio file",
                               juce::File::getSpecialLocation(juce::File::userHomeDirectory),
                               "*.wav");
    
    chooser.launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting, 
                        [this](const juce::FileChooser& fileChooser) {
        if (fileChooser.getResults().size() > 0) {
            juce::File file = fileChooser.getResult();
            controller.saveAudioFile(file.getFullPathName().toStdString());
        }
    });
}

void MainEditor::handleGenerateVariations(const std::string& componentName) {
    // Get selected variation method
    ml::VariationMethod method;
    switch (generationMethodComboBox.getSelectedId()) {
        case 2: method = ml::VariationMethod::GAN; break;
        case 3: method = ml::VariationMethod::VAE; break;
        case 4: method = ml::VariationMethod::STYLE_TRANSFER; break;
        default: method = ml::VariationMethod::ALGORITHMIC;
    }
    
    // Show a progress indicator
    // TODO: Implement proper progress indicator
    juce::AlertWindow::showMessageBoxAsync(
        juce::MessageBoxIconType::InfoIcon,
        "Generating Variations",
        "Generating variations for " + componentName + "...",
        "Cancel");
    
    // Generate variations in a background thread
    juce::Thread::launch([this, componentName, method]() {
        // Generate 4 variations
        std::vector<std::string> variationIds = 
            controller.generateVariations(componentName, method, 4);
        
        // Update UI on the message thread
        juce::MessageManager::callAsync([this, variationIds, componentName]() {
            // Clear existing thumbnails
            variationThumbnails.clear();
            
            // Create thumbnails for each variation
            for (const auto& id : variationIds) {
                // TODO: Create thumbnails for variations
            }
            
            // Show the variation explorer
            if (variationExplorer) {
                variationExplorer->setVisible(true);
                resized();
            }
        });
    });
}

void MainEditor::createComponentUI(const std::string& componentName, juce::Colour colour) {
    ComponentUI ui;
    ui.colour = colour;
    
    // Create audio thumbnail component
    ui.thumbnailComponent = std::make_unique<AudioThumbnailComponent>();
    ui.thumbnail = std::make_unique<juce::AudioThumbnail>(512, /* cache size */
                                                      *formatManager,
                                                      *thumbnailCache);
    ui.thumbnailComponent->setThumbnail(ui.thumbnail.get());
    waveformContainer->addAndMakeVisible(*ui.thumbnailComponent);
    
    // TODO: Set thumbnail source from component audio buffer
    
    // Create sliders
    ui.gainSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                 juce::Slider::TextEntryBoxPosition::NoTextBox);
    ui.gainSlider->setRange(-60.0, 12.0, 0.1);
    ui.gainSlider->setValue(0.0);
    ui.gainSlider->setTooltip("Gain");
    ui.gainSlider->onValueChange = [this, componentName, &ui]() {
        controller.setComponentGain(componentName, ui.gainSlider->getValue());
    };
    waveformContainer->addAndMakeVisible(*ui.gainSlider);
    
    ui.pitchSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                  juce::Slider::TextEntryBoxPosition::NoTextBox);
    ui.pitchSlider->setRange(-12.0, 12.0, 1.0);
    ui.pitchSlider->setValue(0.0);
    ui.pitchSlider->setTooltip("Pitch");
    ui.pitchSlider->onValueChange = [this, componentName, &ui]() {
        controller.setComponentPitchShift(componentName, ui.pitchSlider->getValue());
    };
    waveformContainer->addAndMakeVisible(*ui.pitchSlider);
    
    ui.tempoSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                                  juce::Slider::TextEntryBoxPosition::NoTextBox);
    ui.tempoSlider->setRange(0.5, 2.0, 0.01);
    ui.tempoSlider->setValue(1.0);
    ui.tempoSlider->setTooltip("Tempo");
    ui.tempoSlider->onValueChange = [this, componentName, &ui]() {
        controller.setComponentTempoAdjustment(componentName, ui.tempoSlider->getValue());
    };
    waveformContainer->addAndMakeVisible(*ui.tempoSlider);
    
    ui.effectsComboBox = std::make_unique<juce::ComboBox>();
    ui.effectsComboBox->addItem("None", 1);
    ui.effectsComboBox->addItem("Distortion", 2);
    ui.effectsComboBox->addItem("Reverb", 3);
    ui.effectsComboBox->addItem("Delay", 4);
    ui.effectsComboBox->addItem("Filter", 5);
    ui.effectsComboBox->setSelectedId(1);
    ui.effectsComboBox->onChange = [this, componentName, &ui]() {
        int selectedId = ui.effectsComboBox->getSelectedId();
        if (selectedId > 1) {
            std::string effectName = ui.effectsComboBox->getText().toStdString();
            controller.addComponentEffect(componentName, effectName);
        } else {
            ui.effectsComboBox->setSelectedId(1);
            // Just remove all effects for now since clearComponentEffect doesn't exist
            // TODO: Implement clearComponentEffect in the controller
        }
    };
    waveformContainer->addAndMakeVisible(*ui.effectsComboBox);
    
    ui.soloButton = std::make_unique<juce::TextButton>("S");
    ui.soloButton->setClickingTogglesState(true);
    ui.soloButton->onClick = [this, componentName, &ui]() {
        // TODO: Implement setComponentSolo in the controller
        // For now, we'll just mute all other components if solo is enabled
        bool isSolo = ui.soloButton->getToggleState();
        if (isSolo) {
            // Solo this component by muting all others
            for (const auto& name : controller.getComponentNames()) {
                if (name != componentName) {
                    controller.setComponentEnabled(name, false);
                }
            }
            // Make sure this component is enabled
            controller.setComponentEnabled(componentName, true);
        } else {
            // When un-soloing, enable all components
            for (const auto& name : controller.getComponentNames()) {
                controller.setComponentEnabled(name, true);
            }
        }
    };
    waveformContainer->addAndMakeVisible(*ui.soloButton);
    
    ui.muteButton = std::make_unique<juce::TextButton>("M");
    ui.muteButton->setClickingTogglesState(true);
    ui.muteButton->onClick = [this, componentName, &ui]() {
        controller.setComponentEnabled(componentName, !ui.muteButton->getToggleState());
    };
    waveformContainer->addAndMakeVisible(*ui.muteButton);
    
    ui.generateVariationsButton = std::make_unique<juce::TextButton>("Generate Variations");
    ui.generateVariationsButton->onClick = [this, componentName]() {
        handleGenerateVariations(componentName);
    };
    waveformContainer->addAndMakeVisible(*ui.generateVariationsButton);
    
    // Store the UI
    componentUIs[componentName] = std::move(ui);
}

void MainEditor::createEffectButtons() {
    // Create buttons for each effect
    const std::vector<std::string> effects = controller.getAvailableEffects();
    
    for (const auto& effect : effects) {
        auto button = std::make_unique<juce::DrawableButton>(
            effect, juce::DrawableButton::ButtonStyle::ImageOnButtonBackground);
        
        // TODO: Create proper icons for each effect
        // For now, just use a placeholder
        auto normalImage = std::make_unique<juce::DrawableRectangle>();
        normalImage->setFill(juce::Colours::grey);
        normalImage->setSize(30.0f, 30.0f);
        
        auto hoverImage = std::make_unique<juce::DrawableRectangle>();
        hoverImage->setFill(accentColour);
        hoverImage->setSize(30.0f, 30.0f);
        
        button->setImages(normalImage.get(), hoverImage.get());
        button->setTooltip(effect);
        button->onClick = [this, effect]() {
            // Show effect parameter dialog
            // TODO: Implement effect parameter dialog
        };
        
        addAndMakeVisible(*button);
        effectButtons.push_back(std::move(button));
    }
}

void MainEditor::createVariationExplorerUI() {
    // Create the variation explorer component
    variationExplorer = std::make_unique<juce::Component>();
    variationExplorer->setVisible(false);
    addChildComponent(*variationExplorer);
    
    // TODO: Implement variation explorer UI
}

void MainEditor::createTransportControls() {
    // Create transport source
    transportSource = std::make_unique<juce::AudioTransportSource>();
    
    // Create play button
    playButton = std::make_unique<juce::TextButton>("Play");
    playButton->onClick = [this]() {
        // TODO: Implement playback
    };
    addAndMakeVisible(*playButton);
    
    // Create stop button
    stopButton = std::make_unique<juce::TextButton>("Stop");
    stopButton->onClick = [this]() {
        // TODO: Implement stop
    };
    addAndMakeVisible(*stopButton);
    
    // Create record button
    recordButton = std::make_unique<juce::TextButton>("Rec");
    recordButton->onClick = [this]() {
        // TODO: Implement recording
    };
    addAndMakeVisible(*recordButton);
    
    // Create timeline slider
    timelineSlider = std::make_unique<juce::Slider>(juce::Slider::LinearHorizontal,
                                                 juce::Slider::NoTextBox);
    timelineSlider->setRange(0.0, 1.0);
    timelineSlider->setValue(0.0);
    timelineSlider->setSliderSnapsToMousePosition(false);
    timelineSlider->onValueChange = [this]() {
        // TODO: Implement timeline position change
    };
    addAndMakeVisible(*timelineSlider);
}

void MainEditor::applyDarkTheme() {
    // Set dark theme colors for all components
    
    // Create a custom look and feel for dark theme
    class DarkThemeLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        DarkThemeLookAndFeel() {
            // Set colors for various component types
            setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(20, 20, 24));
            setColour(juce::TextButton::buttonColourId, juce::Colour(40, 40, 45));
            setColour(juce::TextButton::textColourOffId, juce::Colours::white);
            setColour(juce::Slider::thumbColourId, juce::Colour(0, 170, 255));
            setColour(juce::Slider::trackColourId, juce::Colour(60, 60, 70));
            setColour(juce::Slider::backgroundColourId, juce::Colour(30, 30, 35));
            setColour(juce::ComboBox::backgroundColourId, juce::Colour(40, 40, 45));
            setColour(juce::ComboBox::textColourId, juce::Colours::white);
            setColour(juce::ComboBox::arrowColourId, juce::Colour(0, 170, 255));
            setColour(juce::PopupMenu::backgroundColourId, juce::Colour(30, 30, 35));
            setColour(juce::PopupMenu::textColourId, juce::Colours::white);
            setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0, 170, 255));
            setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
            setColour(juce::ScrollBar::thumbColourId, juce::Colour(60, 60, 70));
            setColour(juce::ScrollBar::trackColourId, juce::Colour(30, 30, 35));
            setColour(juce::FileTreeComponent::backgroundColourId, juce::Colour(30, 30, 35));
            setColour(juce::FileTreeComponent::textColourId, juce::Colours::white);
            setColour(juce::DirectoryContentsDisplayComponent::highlightColourId, juce::Colour(0, 170, 255));
        }
    };
    
    // Apply the custom look and feel to all components
    static DarkThemeLookAndFeel darkTheme;
    setLookAndFeel(&darkTheme);
}

} // namespace gui
} // namespace undergroundBeats
