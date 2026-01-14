/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

//==============================================================================
/**
 */
class UYOMPOAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    UYOMPOAudioProcessorEditor(UYOMPOAudioProcessor&);
    ~UYOMPOAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UYOMPOAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UYOMPOAudioProcessorEditor)

    juce::Label captureIniStatusLabel;
};
