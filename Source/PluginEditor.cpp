/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>

//==============================================================================
UYOMPOAudioProcessorEditor::UYOMPOAudioProcessorEditor (UYOMPOAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(captureIniStatusLabel);

    setSize (400, 300);
    startTimerHz(10);
}

UYOMPOAudioProcessorEditor::~UYOMPOAudioProcessorEditor()
{
}

//==============================================================================
void UYOMPOAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void UYOMPOAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    captureIniStatusLabel.setBounds(10, 10, getWidth() - 20, 20);
}

void UYOMPOAudioProcessorEditor::timerCallback()
{
    if (audioProcessor.wasapiCapture == nullptr)
    {
        runTimes++;
        captureIniStatusLabel.setText("runTime: " + std::to_string(runTimes) + " Initializing capture...", juce::dontSendNotification);
        return;
    }
    if (SUCCEEDED(audioProcessor._isRunning.load()))
    {
        captureIniStatusLabel.setText("Succeed init capture", juce::dontSendNotification);
    }

    if (FAILED(audioProcessor._isRunning.load()))
    {
        captureIniStatusLabel.setText("Failed init capture, reason: " + std::to_string(audioProcessor.wasapiCapture->failReason), juce::dontSendNotification);
    }
}