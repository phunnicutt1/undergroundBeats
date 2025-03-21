#include "MainComponent.h"
#include "utils/AudioMath.h"

//==============================================================================
MainComponent::MainComponent() : menuBar(this), tabs(juce::TabbedButtonBar::TabsAtTop)
{
    // Initialize project manager and sequencer
    projectManager = std::make_unique<ProjectManager>();
    projectManager->addListener(this);
    
    // Create a new timeline
    timeline = std::make_shared<Timeline>();
    
    // Initialize sequencer
    sequencer = std::make_unique<Sequencer>();
    sequencer->setTimeline(timeline);
    sequencer->setNoteEventCallback([this](const NoteEvent& event) { handleNoteEvent(event); });
    sequencer->setParameterCallback([this](const std::string& paramId, float value) { handleParameterChange(paramId, value); });
    
    // Initialize MIDI engine
    midiEngine = std::make_unique<MidiEngine>();
    midiEngine->initialize();
    
    // Initialize view components
    mixerView = std::make_unique<MixerView>();
    patternEditorView = std::make_unique<PatternEditorView>();
    
    // Set up the menu bar
    menuBar.setModel(this);
    addAndMakeVisible(menuBar);

    // Set up tab component
    addAndMakeVisible(tabs);
    
    // Add sequencer transport controls
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(recordButton);
    addAndMakeVisible(tempoSlider);
    addAndMakeVisible(tempoLabel);
    
    // Create tab pages
    oscillatorTab = std::make_unique<juce::Component>();
    envelopeTab = std::make_unique<juce::Component>();
    filterTab = std::make_unique<juce::Component>();
    effectsTab = std::make_unique<juce::Component>();
    sequencerTab = std::make_unique<juce::Component>();
    mixerTab = std::make_unique<juce::Component>();
    
    // Add view components to tabs
    mixerTab->addAndMakeVisible(mixerView.get());
    sequencerTab->addAndMakeVisible(patternEditorView.get());
    
    // Add tabs
    tabs.addTab("Oscillator", juce::Colours::darkgrey, oscillatorTab.get(), false);
    tabs.addTab("Envelope", juce::Colours::darkgrey, envelopeTab.get(), false);
    tabs.addTab("Filter", juce::Colours::darkgrey, filterTab.get(), false);
    tabs.addTab("Effects", juce::Colours::darkgrey, effectsTab.get(), false);
    tabs.addTab("Sequencer", juce::Colours::darkgrey, sequencerTab.get(), false);
    tabs.addTab("Mixer", juce::Colours::darkgrey, mixerTab.get(), false);
    
    // Add transport controls directly to main component
    addAndMakeVisible(startButton);
    
    // Configure start button
    startButton.onClick = [this]() {
        if (audioEngine.isRunning()) {
            audioEngine.stop();
            startButton.setButtonText("Start Engine");
        } else {
            audioEngine.start();
            startButton.setButtonText("Stop Engine");
        }
    };
    
    // Configure sequencer transport controls
    playButton.onClick = [this]() {
        sequencer->play();
    };
    
    stopButton.onClick = [this]() {
        sequencer->stop();
    };
    
    recordButton.onClick = [this]() {
        // Toggle record mode
        recordButton.setToggleState(!recordButton.getToggleState(), juce::sendNotification);
    };
    
    // Configure tempo slider
    tempoSlider.setRange(40.0, 240.0, 1.0);
    tempoSlider.setValue(120.0);
    tempoSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    tempoSlider.onValueChange = [this]() {
        if (sequencer)
            sequencer->setTempo(tempoSlider.getValue());
    };
    
    // === OSCILLATOR TAB ===
    oscillatorTab->addAndMakeVisible(frequencySlider);
    oscillatorTab->addAndMakeVisible(frequencyLabel);
    oscillatorTab->addAndMakeVisible(waveformSelector);
    oscillatorTab->addAndMakeVisible(waveformLabel);
    oscillatorTab->addAndMakeVisible(pulseWidthSlider);
    oscillatorTab->addAndMakeVisible(pulseWidthLabel);
    oscillatorTab->addAndMakeVisible(detuneSlider);
    oscillatorTab->addAndMakeVisible(detuneLabel);
    oscillatorTab->addAndMakeVisible(gainSlider);
    oscillatorTab->addAndMakeVisible(gainLabel);
    
    // Configure oscillator controls
    frequencySlider.setRange(20.0, 20000.0, 0.1);
    frequencySlider.setSkewFactorFromMidPoint(1000.0); // Logarithmic scaling
    frequencySlider.setValue(440.0);
    frequencySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    frequencySlider.onValueChange = [this]() {
        if (oscillator)
            oscillator->setFrequency(static_cast<float>(frequencySlider.getValue()));
    };
    
    waveformSelector.addItem("Sine", Oscillator::Sine + 1);
    waveformSelector.addItem("Triangle", Oscillator::Triangle + 1);
    waveformSelector.addItem("Sawtooth", Oscillator::Sawtooth + 1);
    waveformSelector.addItem("Square", Oscillator::Square + 1);
    waveformSelector.addItem("Noise", Oscillator::Noise + 1);
    waveformSelector.setSelectedId(Oscillator::Sine + 1);
    waveformSelector.onChange = [this]() {
        if (oscillator)
            oscillator->setWaveform(static_cast<Oscillator::WaveformType>(waveformSelector.getSelectedId() - 1));
    };
    
    pulseWidthSlider.setRange(0.01, 0.99, 0.01);
    pulseWidthSlider.setValue(0.5);
    pulseWidthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    pulseWidthSlider.onValueChange = [this]() {
        if (oscillator)
            oscillator->setPulseWidth(static_cast<float>(pulseWidthSlider.getValue()));
    };
    
    detuneSlider.setRange(-100.0, 100.0, 0.1);
    detuneSlider.setValue(0.0);
    detuneSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    detuneSlider.onValueChange = [this]() {
        if (oscillator)
            oscillator->setDetune(static_cast<float>(detuneSlider.getValue()));
    };
    
    gainSlider.setRange(0.0, 1.0, 0.01);
    gainSlider.setValue(0.5);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    gainSlider.onValueChange = [this]() {
        if (oscillator)
            oscillator->setGain(static_cast<float>(gainSlider.getValue()));
    };
    
    // === ENVELOPE TAB ===
    envelopeTab->addAndMakeVisible(attackSlider);
    envelopeTab->addAndMakeVisible(attackLabel);
    envelopeTab->addAndMakeVisible(decaySlider);
    envelopeTab->addAndMakeVisible(decayLabel);
    envelopeTab->addAndMakeVisible(sustainSlider);
    envelopeTab->addAndMakeVisible(sustainLabel);
    envelopeTab->addAndMakeVisible(releaseSlider);
    envelopeTab->addAndMakeVisible(releaseLabel);
    envelopeTab->addAndMakeVisible(triggerButton);
    
    // Configure envelope controls
    attackSlider.setRange(0.1, 5000.0, 0.1);
    attackSlider.setSkewFactorFromMidPoint(500.0);
    attackSlider.setValue(10.0);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    attackSlider.onValueChange = [this]() {
        if (envelope)
            envelope->setAttack(static_cast<float>(attackSlider.getValue()));
    };
    
    decaySlider.setRange(0.1, 5000.0, 0.1);
    decaySlider.setSkewFactorFromMidPoint(500.0);
    decaySlider.setValue(100.0);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    decaySlider.onValueChange = [this]() {
        if (envelope)
            envelope->setDecay(static_cast<float>(decaySlider.getValue()));
    };
    
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.7);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    sustainSlider.onValueChange = [this]() {
        if (envelope)
            envelope->setSustain(static_cast<float>(sustainSlider.getValue()));
    };
    
    releaseSlider.setRange(0.1, 10000.0, 0.1);
    releaseSlider.setSkewFactorFromMidPoint(1000.0);
    releaseSlider.setValue(200.0);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    releaseSlider.onValueChange = [this]() {
        if (envelope)
            envelope->setRelease(static_cast<float>(releaseSlider.getValue()));
    };
    
    triggerButton.onClick = [this]() {
        if (envelope) {
            envelope->noteOn();
            startTimer(500); // Timer to auto-release after 500ms
        }
    };
    
    // === FILTER TAB ===
    filterTab->addAndMakeVisible(filterFreqSlider);
    filterTab->addAndMakeVisible(filterFreqLabel);
    filterTab->addAndMakeVisible(resonanceSlider);
    filterTab->addAndMakeVisible(resonanceLabel);
    filterTab->addAndMakeVisible(filterTypeSelector);
    filterTab->addAndMakeVisible(filterTypeLabel);
    
    // Configure filter controls
    filterFreqSlider.setRange(20.0, 20000.0, 0.1);
    filterFreqSlider.setSkewFactorFromMidPoint(1000.0);
    filterFreqSlider.setValue(1000.0);
    filterFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    filterFreqSlider.onValueChange = [this]() {
        if (filter)
            filter->setFrequency(static_cast<float>(filterFreqSlider.getValue()));
    };
    
    resonanceSlider.setRange(0.1, 10.0, 0.01);
    resonanceSlider.setValue(0.7071);
    resonanceSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    resonanceSlider.onValueChange = [this]() {
        if (filter)
            filter->setResonance(static_cast<float>(resonanceSlider.getValue()));
    };
    
    filterTypeSelector.addItem("Low Pass", Filter::LowPass + 1);
    filterTypeSelector.addItem("High Pass", Filter::HighPass + 1);
    filterTypeSelector.addItem("Band Pass", Filter::BandPass + 1);
    filterTypeSelector.addItem("Notch", Filter::Notch + 1);
    filterTypeSelector.addItem("Low Shelf", Filter::LowShelf + 1);
    filterTypeSelector.addItem("High Shelf", Filter::HighShelf + 1);
    filterTypeSelector.addItem("Peak", Filter::Peak + 1);
    filterTypeSelector.setSelectedId(Filter::LowPass + 1);
    filterTypeSelector.onChange = [this]() {
        if (filter)
            filter->setFilterType(static_cast<Filter::FilterType>(filterTypeSelector.getSelectedId() - 1));
    };
    
    // === EFFECTS TAB ===
    effectsTab->addAndMakeVisible(effectSelector);
    effectsTab->addAndMakeVisible(effectLabel);
    
    // Delay controls
    effectsTab->addAndMakeVisible(delayTimeSlider);
    effectsTab->addAndMakeVisible(delayTimeLabel);
    effectsTab->addAndMakeVisible(feedbackSlider);
    effectsTab->addAndMakeVisible(feedbackLabel);
    effectsTab->addAndMakeVisible(delayMixSlider);
    effectsTab->addAndMakeVisible(delayMixLabel);
    
    // Reverb controls
    effectsTab->addAndMakeVisible(roomSizeSlider);
    effectsTab->addAndMakeVisible(roomSizeLabel);
    effectsTab->addAndMakeVisible(dampingSlider);
    effectsTab->addAndMakeVisible(dampingLabel);
    effectsTab->addAndMakeVisible(widthSlider);
    effectsTab->addAndMakeVisible(widthLabel);
    effectsTab->addAndMakeVisible(reverbMixSlider);
    effectsTab->addAndMakeVisible(reverbMixLabel);
    
    // Configure effect selector
    effectSelector.addItem("No Effect", NoEffect + 1);
    effectSelector.addItem("Delay", DelayEffect + 1);
    effectSelector.addItem("Reverb", ReverbEffect + 1);
    effectSelector.setSelectedId(NoEffect + 1);
    effectSelector.onChange = [this]() {
        currentEffect = static_cast<EffectType>(effectSelector.getSelectedId() - 1);
        updateEffectsUI();
    };
    
    // Configure delay controls
    delayTimeSlider.setRange(10.0, 2000.0, 1.0);
    delayTimeSlider.setValue(500.0);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    delayTimeSlider.onValueChange = [this]() {
        if (delay)
            delay->setDelayTime(static_cast<float>(delayTimeSlider.getValue()));
    };
    
    feedbackSlider.setRange(0.0, 0.95, 0.01);
    feedbackSlider.setValue(0.5);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    feedbackSlider.onValueChange = [this]() {
        if (delay)
            delay->setFeedback(static_cast<float>(feedbackSlider.getValue()));
    };
    
    delayMixSlider.setRange(0.0, 1.0, 0.01);
    delayMixSlider.setValue(0.3);
    delayMixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    delayMixSlider.onValueChange = [this]() {
        if (delay)
            delay->setDryWet(static_cast<float>(delayMixSlider.getValue()));
    };
    
    // Configure reverb controls
    roomSizeSlider.setRange(0.0, 1.0, 0.01);
    roomSizeSlider.setValue(0.5);
    roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    roomSizeSlider.onValueChange = [this]() {
        if (reverb)
            reverb->setRoomSize(static_cast<float>(roomSizeSlider.getValue()));
    };
    
    dampingSlider.setRange(0.0, 1.0, 0.01);
    dampingSlider.setValue(0.5);
    dampingSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    dampingSlider.onValueChange = [this]() {
        if (reverb)
            reverb->setDamping(static_cast<float>(dampingSlider.getValue()));
    };
    
    widthSlider.setRange(0.0, 1.0, 0.01);
    widthSlider.setValue(1.0);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    widthSlider.onValueChange = [this]() {
        if (reverb)
            reverb->setWidth(static_cast<float>(widthSlider.getValue()));
    };
    
    reverbMixSlider.setRange(0.0, 1.0, 0.01);
    reverbMixSlider.setValue(0.3);
    reverbMixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    reverbMixSlider.onValueChange = [this]() {
        if (reverb)
            reverb->setDryWet(static_cast<float>(reverbMixSlider.getValue()));
    };
    
    // Initial UI update
    updateEffectsUI();
    
    // Configure component size
    setSize(800, 600);
    
    // Request audio permissions
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    // Remove change listeners
    projectManager->removeListener(this);
    
    // Shut down the audio
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    // Initialize the audio engine
    AudioDeviceSettings settings;
    settings.sampleRate = sampleRate;
    settings.bufferSize = samplesPerBlockExpected;
    settings.outputChannels = 2;
    
    audioEngine.initialize(settings);
    
    // Create all processors
    createOscillator();
    createEnvelope();
    createFilter();
    createDelay();
    createReverb();
    
    // Connect them in the processing chain
    connectProcessors();
    
    // Prepare the sequencer
    sequencer->prepare(sampleRate, samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Process MIDI from the sequencer
    juce::MidiBuffer midiBuffer;
    juce::MidiBuffer emptyBuffer;
    
    // Get MIDI messages from the sequencer
    if (sequencer) {
        sequencer->processMidi(emptyBuffer, midiBuffer);
    }
    
    // Pass MIDI messages to the synthesizer (via audio engine later)
    // For now we're just using it to trigger the oscillator directly
    if (!midiBuffer.isEmpty()) {
        juce::MidiBuffer::Iterator iterator(midiBuffer);
        juce::MidiMessage message;
        int samplePosition;
        
        while (iterator.getNextEvent(message, samplePosition)) {
            if (message.isNoteOn()) {
                if (oscillator) {
                    oscillator->setFrequency(AudioMath::midiNoteToFrequency(message.getNoteNumber()));
                }
                if (envelope) {
                    envelope->noteOn();
                }
            } else if (message.isNoteOff()) {
                if (envelope) {
                    envelope->noteOff();
                }
            }
        }
    }
    
    // Let the engine handle the audio processing
    audioEngine.processAudio(bufferToFill);
}

void MainComponent::releaseResources()
{
    // Stop sequencer
    if (sequencer) {
        sequencer->stop();
    }
    
    // Shut down the engine
    audioEngine.shutdown();
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Title
    g.setFont(24.0f);
    g.setColour(juce::Colours::white);
    g.drawText("Underground Beats", getLocalBounds().removeFromTop(40),
                juce::Justification::centred, true);
    
    // Status
    g.setFont(16.0f);
    g.setColour(juce::Colours::lightgrey);
    g.drawText("Audio Engine: " + juce::String(audioEngine.isRunning() ? "Running" : "Stopped"),
                getLocalBounds().removeFromTop(80).removeFromBottom(20),
                juce::Justification::centred, true);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    
    // Menu bar
    menuBar.setBounds(area.removeFromTop(25));
    
    // Set margins
    area = area.reduced(20);
    
    // Title space
    area.removeFromTop(40);
    
    // Transport controls row
    auto transportRow = area.removeFromTop(40);
    startButton.setBounds(transportRow.removeFromLeft(150).reduced(5));
    playButton.setBounds(transportRow.removeFromLeft(80).reduced(5));
    stopButton.setBounds(transportRow.removeFromLeft(80).reduced(5));
    recordButton.setBounds(transportRow.removeFromLeft(80).reduced(5));
    
    tempoLabel.setBounds(transportRow.removeFromLeft(100).reduced(5));
    tempoSlider.setBounds(transportRow.reduced(5));
    
    // Space between controls
    area.removeFromTop(20);
    
    // Tabs take the rest of the space
    tabs.setBounds(area);
    
    // Resize components inside tabs
    if (mixerView)
    {
        mixerView->setBounds(mixerTab->getLocalBounds());
    }
    
    if (patternEditorView)
    {
        patternEditorView->setBounds(sequencerTab->getLocalBounds());
    }
    
    // Layout for oscillator tab
    auto oscillatorArea = oscillatorTab->getLocalBounds().reduced(10);
    const int controlHeight = 30;
    const int labelWidth = 150;
    
    // Oscillator tab layout
    // Frequency controls
    auto rowArea = oscillatorArea.removeFromTop(controlHeight);
    frequencyLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    frequencySlider.setBounds(rowArea);
    
    // Waveform controls
    oscillatorArea.removeFromTop(10);
    rowArea = oscillatorArea.removeFromTop(controlHeight);
    waveformLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    waveformSelector.setBounds(rowArea.removeFromLeft(200));
    
    // Pulse width controls
    oscillatorArea.removeFromTop(10);
    rowArea = oscillatorArea.removeFromTop(controlHeight);
    pulseWidthLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    pulseWidthSlider.setBounds(rowArea);
    
    // Detune controls
    oscillatorArea.removeFromTop(10);
    rowArea = oscillatorArea.removeFromTop(controlHeight);
    detuneLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    detuneSlider.setBounds(rowArea);
    
    // Gain controls
    oscillatorArea.removeFromTop(10);
    rowArea = oscillatorArea.removeFromTop(controlHeight);
    gainLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    gainSlider.setBounds(rowArea);
    
    // Layout for envelope tab
    auto envelopeArea = envelopeTab->getLocalBounds().reduced(10);
    
    // Attack controls
    rowArea = envelopeArea.removeFromTop(controlHeight);
    attackLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    attackSlider.setBounds(rowArea);
    
    // Decay controls
    envelopeArea.removeFromTop(10);
    rowArea = envelopeArea.removeFromTop(controlHeight);
    decayLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    decaySlider.setBounds(rowArea);
    
    // Sustain controls
    envelopeArea.removeFromTop(10);
    rowArea = envelopeArea.removeFromTop(controlHeight);
    sustainLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    sustainSlider.setBounds(rowArea);
    
    // Release controls
    envelopeArea.removeFromTop(10);
    rowArea = envelopeArea.removeFromTop(controlHeight);
    releaseLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    releaseSlider.setBounds(rowArea);
    
    // Trigger button
    envelopeArea.removeFromTop(20);
    triggerButton.setBounds(envelopeArea.removeFromTop(40).withSizeKeepingCentre(200, 40));
    
    // Layout for filter tab
    auto filterArea = filterTab->getLocalBounds().reduced(10);
    
    // Filter frequency controls
    rowArea = filterArea.removeFromTop(controlHeight);
    filterFreqLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    filterFreqSlider.setBounds(rowArea);
    
    // Resonance controls
    filterArea.removeFromTop(10);
    rowArea = filterArea.removeFromTop(controlHeight);
    resonanceLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    resonanceSlider.setBounds(rowArea);
    
    // Filter type controls
    filterArea.removeFromTop(10);
    rowArea = filterArea.removeFromTop(controlHeight);
    filterTypeLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    filterTypeSelector.setBounds(rowArea.removeFromLeft(200));
    
    // Layout for effects tab
    auto effectsArea = effectsTab->getLocalBounds().reduced(10);
    
    // Effect selector
    rowArea = effectsArea.removeFromTop(controlHeight);
    effectLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    effectSelector.setBounds(rowArea.removeFromLeft(200));
    
    effectsArea.removeFromTop(20);
    
    // Delay controls
    auto delayArea = effectsArea.removeFromTop(150);
    
    rowArea = delayArea.removeFromTop(controlHeight);
    delayTimeLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    delayTimeSlider.setBounds(rowArea);
    
    delayArea.removeFromTop(10);
    rowArea = delayArea.removeFromTop(controlHeight);
    feedbackLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    feedbackSlider.setBounds(rowArea);
    
    delayArea.removeFromTop(10);
    rowArea = delayArea.removeFromTop(controlHeight);
    delayMixLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    delayMixSlider.setBounds(rowArea);
    
    // Reverb controls
    effectsArea.removeFromTop(20);
    auto reverbArea = effectsArea.removeFromTop(200);
    
    rowArea = reverbArea.removeFromTop(controlHeight);
    roomSizeLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    roomSizeSlider.setBounds(rowArea);
    
    reverbArea.removeFromTop(10);
    rowArea = reverbArea.removeFromTop(controlHeight);
    dampingLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    dampingSlider.setBounds(rowArea);
    
    reverbArea.removeFromTop(10);
    rowArea = reverbArea.removeFromTop(controlHeight);
    widthLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    widthSlider.setBounds(rowArea);
    
    reverbArea.removeFromTop(10);
    rowArea = reverbArea.removeFromTop(controlHeight);
    reverbMixLabel.setBounds(rowArea.removeFromLeft(labelWidth));
    reverbMixSlider.setBounds(rowArea);
}

void MainComponent::timerCallback()
{
    // Auto-release for envelope trigger
    if (envelope) {
        envelope->noteOff();
        stopTimer();
    }
}

void MainComponent::createOscillator()
{
    // Create a new oscillator
    oscillator = std::make_unique<Oscillator>();
    
    // Configure it based on UI settings
    oscillator->setFrequency(static_cast<float>(frequencySlider.getValue()));
    oscillator->setWaveform(static_cast<Oscillator::WaveformType>(waveformSelector.getSelectedId() - 1));
    oscillator->setPulseWidth(static_cast<float>(pulseWidthSlider.getValue()));
    oscillator->setDetune(static_cast<float>(detuneSlider.getValue()));
    oscillator->setGain(static_cast<float>(gainSlider.getValue()));
    
    // Add it to the engine
    oscillatorNodeId = audioEngine.addProcessor(std::move(oscillator));
}

void MainComponent::createEnvelope()
{
    // Create a new envelope
    envelope = std::make_unique<Envelope>();
    
    // Configure it based on UI settings
    envelope->setAttack(static_cast<float>(attackSlider.getValue()));
    envelope->setDecay(static_cast<float>(decaySlider.getValue()));
    envelope->setSustain(static_cast<float>(sustainSlider.getValue()));
    envelope->setRelease(static_cast<float>(releaseSlider.getValue()));
    
    // Add it to the engine
    envelopeNodeId = audioEngine.addProcessor(std::move(envelope));
}

void MainComponent::createFilter()
{
    // Create a new filter
    filter = std::make_unique<Filter>();
    
    // Configure it based on UI settings
    filter->setFrequency(static_cast<float>(filterFreqSlider.getValue()));
    filter->setResonance(static_cast<float>(resonanceSlider.getValue()));
    filter->setFilterType(static_cast<Filter::FilterType>(filterTypeSelector.getSelectedId() - 1));
    
    // Add it to the engine
    filterNodeId = audioEngine.addProcessor(std::move(filter));
}

void MainComponent::createDelay()
{
    // Create a new delay
    delay = std::make_unique<Delay>();
    
    // Configure it based on UI settings
    delay->setDelayTime(static_cast<float>(delayTimeSlider.getValue()));
    delay->setFeedback(static_cast<float>(feedbackSlider.getValue()));
    delay->setDryWet(static_cast<float>(delayMixSlider.getValue()));
    
    // Add it to the engine
    delayNodeId = audioEngine.addProcessor(std::move(delay));
}

void MainComponent::createReverb()
{
    // Create a new reverb
    reverb = std::make_unique<Reverb>();
    
    // Configure it based on UI settings
    reverb->setRoomSize(static_cast<float>(roomSizeSlider.getValue()));
    reverb->setDamping(static_cast<float>(dampingSlider.getValue()));
    reverb->setWidth(static_cast<float>(widthSlider.getValue()));
    reverb->setDryWet(static_cast<float>(reverbMixSlider.getValue()));
    
    // Add it to the engine
    reverbNodeId = audioEngine.addProcessor(std::move(reverb));
}

void MainComponent::connectProcessors()
{
    // Connect oscillator to envelope
    audioEngine.connectNodes(oscillatorNodeId, 0, envelopeNodeId, 0);
    audioEngine.connectNodes(oscillatorNodeId, 1, envelopeNodeId, 1);
    
    // Connect envelope to filter
    audioEngine.connectNodes(envelopeNodeId, 0, filterNodeId, 0);
    audioEngine.connectNodes(envelopeNodeId, 1, filterNodeId, 1);
    
    // Connect based on selected effect
    switch (currentEffect)
    {
        case DelayEffect:
            // Connect filter to delay
            audioEngine.connectNodes(filterNodeId, 0, delayNodeId, 0);
            audioEngine.connectNodes(filterNodeId, 1, delayNodeId, 1);
            break;
            
        case ReverbEffect:
            // Connect filter to reverb
            audioEngine.connectNodes(filterNodeId, 0, reverbNodeId, 0);
            audioEngine.connectNodes(filterNodeId, 1, reverbNodeId, 1);
            break;
            
        case NoEffect:
        default:
            // Do nothing - filter outputs directly
            break;
    }
}

void MainComponent::updateEffectsUI()
{
    // Show/hide controls based on the selected effect
    bool showDelay = (currentEffect == DelayEffect);
    bool showReverb = (currentEffect == ReverbEffect);
    
    // Delay controls
    delayTimeSlider.setVisible(showDelay);
    delayTimeLabel.setVisible(showDelay);
    feedbackSlider.setVisible(showDelay);
    feedbackLabel.setVisible(showDelay);
    delayMixSlider.setVisible(showDelay);
    delayMixLabel.setVisible(showDelay);
    
    // Reverb controls
    roomSizeSlider.setVisible(showReverb);
    roomSizeLabel.setVisible(showReverb);
    dampingSlider.setVisible(showReverb);
    dampingLabel.setVisible(showReverb);
    widthSlider.setVisible(showReverb);
    widthLabel.setVisible(showReverb);
    reverbMixSlider.setVisible(showReverb);
    reverbMixLabel.setVisible(showReverb);
    
    // Reconnect processors based on the current effect
    connectProcessors();
}

// Menu creation and handling
juce::StringArray MainComponent::getMenuBarNames()
{
    return { "File", "Edit", "View", "Project", "Help" };
}

juce::PopupMenu MainComponent::getMenuForIndex(int menuIndex, const juce::String& menuName)
{
    juce::PopupMenu menu;
    
    if (menuIndex == 0) // File menu
    {
        menu.addItem(CommandIDs::NewProject, "New Project");
        menu.addItem(CommandIDs::OpenProject, "Open Project...");
        menu.addSeparator();
        menu.addItem(CommandIDs::SaveProject, "Save Project");
        menu.addItem(CommandIDs::SaveProjectAs, "Save Project As...");
        menu.addSeparator();
        menu.addItem(CommandIDs::ExportAudio, "Export Audio...");
        menu.addSeparator();
        menu.addItem(CommandIDs::Preferences, "Preferences...");
        menu.addSeparator();
        menu.addItem(9999, "Exit");
    }
    else if (menuIndex == 1) // Edit menu
    {
        menu.addItem(CommandIDs::Undo, "Undo");
        menu.addItem(CommandIDs::Redo, "Redo");
        menu.addSeparator();
        menu.addItem(CommandIDs::Cut, "Cut");
        menu.addItem(CommandIDs::Copy, "Copy");
        menu.addItem(CommandIDs::Paste, "Paste");
        menu.addItem(CommandIDs::Delete, "Delete");
        menu.addSeparator();
        menu.addItem(CommandIDs::SelectAll, "Select All");
    }
    else if (menuIndex == 2) // View menu
    {
        menu.addItem(201, "Mixer");
        menu.addItem(202, "Pattern Editor");
        menu.addItem(203, "Instrument Editor");
    }
    else if (menuIndex == 3) // Project menu
    {
        menu.addItem(CommandIDs::AddPattern, "Add Pattern");
        menu.addItem(CommandIDs::AddTrack, "Add Track");
        menu.addSeparator();
        menu.addItem(CommandIDs::DeletePattern, "Delete Pattern");
        menu.addItem(CommandIDs::DeleteTrack, "Delete Track");
    }
    else if (menuIndex == 4) // Help menu
    {
        menu.addItem(401, "About Underground Beats");
        menu.addItem(402, "User Manual");
    }
    
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    switch (menuItemID)
    {
        case CommandIDs::NewProject:
            newProject();
            break;
            
        case CommandIDs::OpenProject:
            openProject();
            break;
            
        case CommandIDs::SaveProject:
            saveProject(false);
            break;
            
        case CommandIDs::SaveProjectAs:
            saveProject(true);
            break;
            
        case CommandIDs::ExportAudio:
            exportAudio();
            break;
            
        case 9999: // Exit
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
            break;
            
        default:
            // Handle other menu items
            break;
    }
}

// Project management
void MainComponent::newProject()
{
    if (projectManager->hasUnsavedChanges())
    {
        // In a real implementation, we would prompt the user to save
        // For now, we'll just create a new project
    }
    
    // Create a new project
    projectManager->createNewProject("New Project", 44100.0);
    
    // Create a new timeline
    timeline = std::make_shared<Timeline>();
    sequencer->setTimeline(timeline);
}

void MainComponent::openProject()
{
    juce::FileChooser fileChooser("Open Project", juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "*.ubp");
    
    if (fileChooser.browseForFileToOpen())
    {
        juce::File file = fileChooser.getResult();
        projectManager->loadProject(file);
    }
}

void MainComponent::saveProject(bool saveAs)
{
    projectManager->saveProject(saveAs);
}

void MainComponent::exportAudio()
{
    // This would be implemented later
}

// Handle sequencer events
void MainComponent::handleNoteEvent(const NoteEvent& event)
{
    // Handle note events from the sequencer
    if (event.velocity > 0) // Note on
    {
        if (oscillator)
        {
            oscillator->setFrequency(AudioMath::midiNoteToFrequency(event.note));
        }
        if (envelope)
        {
            envelope->noteOn();
        }
    }
    else // Note off
    {
        if (envelope)
        {
            envelope->noteOff();
        }
    }
}

void MainComponent::handleParameterChange(const std::string& paramId, float value)
{
    // Handle parameter automation from the sequencer
    // This would be implemented to map parameter IDs to processor parameters
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // Handle changes from project manager
    if (source == projectManager.get())
    {
        // Update sequencer with project state
        ProjectState& projectState = projectManager->getProjectState();
        
        // Update tempo
        sequencer->setTempo(projectState.getSettings().tempo);
        tempoSlider.setValue(projectState.getSettings().tempo, juce::dontSendNotification);
        
        // Update time signature
        sequencer->setTimeSignature(projectState.getSettings().timeSignatureNumerator, 
                                 projectState.getSettings().timeSignatureDenominator);
        
        // Update UI
        repaint();
    }
}
