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
    addAndMakeVisible(captureFmt);

    setSize (800, 300);
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
    captureFmt.setBounds(10, 40, getWidth() - 20, getHeight() - 50);
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
        captureIniStatusLabel.setText("Succeed init capture, now volume is " + std::to_string(audioProcessor.wasapiCapture->capturedVolume) + "fail flag is " + std::to_string(audioProcessor.wasapiCapture->failReason), juce::dontSendNotification);
    }

    if (FAILED(audioProcessor._isRunning.load()))
    {
        captureIniStatusLabel.setText("Failed init capture, reason: " + std::to_string(audioProcessor.wasapiCapture->failReason), juce::dontSendNotification);
    }

    auto* fmt = audioProcessor.wasapiCapture->mixFmt;

    std::string text;

    text += "wFormatTag = " + std::to_string(fmt->wFormatTag) + "\n";
    text += "nChannels = " + std::to_string(fmt->nChannels) + "\n";
    text += "nSamplesPerSec = " + std::to_string(fmt->nSamplesPerSec) + "\n";
    text += "wBitsPerSample = " + std::to_string(fmt->wBitsPerSample) + "\n";
    text += "nBlockAlign = " + std::to_string(fmt->nBlockAlign) + "\n";
    text += "nAvgBytesPerSec = " + std::to_string(fmt->nAvgBytesPerSec) + "\n";
    text += "cbSize = " + std::to_string(fmt->cbSize) + "\n";

    if (fmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        auto* ext = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(fmt);

        text += "---- EXTENSIBLE ----\n";
        text += "ValidBits = " + std::to_string(ext->Samples.wValidBitsPerSample) + "\n";
        text += "ChannelMask = " + std::to_string(ext->dwChannelMask) + "\n";

        if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
            text += "SubFormat = IEEE_FLOAT\n";
        else if (ext->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
            text += "SubFormat = PCM\n";
        else
            text += "SubFormat = OTHER\n";
    }

    captureFmt.setText(text, juce::dontSendNotification);
}