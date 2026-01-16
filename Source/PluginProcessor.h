/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "WASAPI/WasapiCapture.h"
#include <JuceHeader.h>
#include <atomic>
#include "SharedRingBuffer/SharedRingBuffer.h"

//==============================================================================
/**
 */
class UYOMPOAudioProcessor : public juce::AudioProcessor {
public:
    //==============================================================================
    UYOMPOAudioProcessor();
    ~UYOMPOAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    std::atomic<HRESULT> _isRunning { E_FAIL };
    std::unique_ptr<WasapiCapture> wasapiCapture;
    SharedRingBuffer sharedRingBuffer; // 10 seconds buffer at 48kHz, stereo
    std::atomic<int> juceBufferSize { 0 };
    std::atomic<int> juceSampleRate { 0 };
    std::atomic<int> numReady { 0 };

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UYOMPOAudioProcessor);

    juce::AudioBuffer<float> tempBuffer;

    std::vector<juce::LagrangeInterpolator> interpolators;
    double hostRate = 44100.0;
    double wasapiRate = 48000;
    double speedRatio = 48000.0 / 44100.0;
    int maxInNeeded = 0;
};
