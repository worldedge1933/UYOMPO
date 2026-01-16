/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SharedRingBuffer/SharedRingBuffer.h"

//==============================================================================
UYOMPOAudioProcessor::UYOMPOAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
              )
    , sharedRingBuffer(1200, 2)
#endif
{

    std::thread([this]() {
        this->wasapiCapture = std::make_unique<WasapiCapture>(this->_isRunning, &this->sharedRingBuffer);
        wasapiCapture->initialize();
        wasapiCapture->startCapture();
    }).detach();
}

UYOMPOAudioProcessor::~UYOMPOAudioProcessor()
{
}

//==============================================================================
const juce::String UYOMPOAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UYOMPOAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool UYOMPOAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool UYOMPOAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double UYOMPOAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UYOMPOAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int UYOMPOAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UYOMPOAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String UYOMPOAudioProcessor::getProgramName(int index)
{
    return {};
}

void UYOMPOAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void UYOMPOAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    if (SUCCEEDED(this->_isRunning.load())) {
    }

    juceBufferSize.store(samplesPerBlock);
    juceSampleRate.store(static_cast<int>(sampleRate));

    tempBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock * 2);

    hostRate = sampleRate;
    if (wasapiCapture == nullptr || wasapiCapture->mixFmt == nullptr) {
        wasapiRate = 48000.0;
    } else {
        wasapiRate = wasapiCapture->mixFmt->nSamplesPerSec;
    }
    speedRatio = wasapiRate / hostRate;
    maxInNeeded = static_cast<int>(std::ceil(speedRatio * samplesPerBlock)) + 2;

    interpolators.clear();
    interpolators.resize(getTotalNumOutputChannels());
    for (auto& interp : interpolators) {
        interp.reset();
    }
}

void UYOMPOAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UYOMPOAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void UYOMPOAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    numReady.store(sharedRingBuffer.getNumReady());
    if (numReady.load() < maxInNeeded * 1.1) {
        speedRatio -= 0.0001;
    } else {
        speedRatio += 0.0001;
    }

    sharedRingBuffer.popToAudioProcessBlock(tempBuffer, (int)(buffer.getNumSamples() * speedRatio));

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        interpolators[channel].process(speedRatio,
            tempBuffer.getReadPointer(channel),
            channelData,
            buffer.getNumSamples());

        // ..do something to the data...
    }
    
}

//==============================================================================
bool UYOMPOAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UYOMPOAudioProcessor::createEditor()
{
    return new UYOMPOAudioProcessorEditor(*this);
}

//==============================================================================
void UYOMPOAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void UYOMPOAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UYOMPOAudioProcessor();
}
