/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>

//==============================================================================
UYOMPOAudioProcessorEditor::UYOMPOAudioProcessorEditor(UYOMPOAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(captureIniStatusLabel);
    addAndMakeVisible(failReasonLabel);
    addAndMakeVisible(captureFmtLabel);
    addAndMakeVisible(capturedFramesLabel);
    addAndMakeVisible(juceSampleRate);
    addAndMakeVisible(juceBufferSize);

    setSize(500, 500);
    startTimerHz(10);
}

UYOMPOAudioProcessorEditor::~UYOMPOAudioProcessorEditor()
{
}

//==============================================================================
void UYOMPOAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));
}

void UYOMPOAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int beginHeight = 10;
    captureIniStatusLabel.setBounds(10, beginHeight, getWidth() - 20, 20);

    beginHeight += 40;
    failReasonLabel.setBounds(10, beginHeight, getWidth() - 20, 20);

    beginHeight += 40;
    captureFmtLabel.setBounds(10, beginHeight, getWidth() - 20, 180);

    beginHeight += 200;
    capturedFramesLabel.setBounds(10, beginHeight, getWidth() - 20, 20);

    beginHeight += 40;
    juceBufferSize.setBounds(10, beginHeight, getWidth() - 20, 20);

    beginHeight += 40;
    juceSampleRate.setBounds(10, beginHeight, getWidth() - 20, 20);
}

void UYOMPOAudioProcessorEditor::timerCallback()
{
    if (audioProcessor.wasapiCapture == nullptr) {
        runTimes++;
        captureIniStatusLabel.setText("runTime: " + std::to_string(runTimes) + " Initializing capture...", juce::dontSendNotification);
        return;
    }
    if (SUCCEEDED(audioProcessor._isRunning.load())) {
        captureIniStatusLabel.setText("Succeed init capture, now volume is " + std::to_string(audioProcessor.wasapiCapture->capturedVolume), juce::dontSendNotification);
    }

    if (FAILED(audioProcessor._isRunning.load())) {
        captureIniStatusLabel.setText("Failed init capture", juce::dontSendNotification);
    }

    failReasonLabel.setText("Fail flag is: " + std::to_string(audioProcessor.wasapiCapture->failReason), juce::dontSendNotification);

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

    capturedFramesLabel.setText("Captured Frames: " + std::to_string(audioProcessor.wasapiCapture->capturedFrames.load()), juce::dontSendNotification);

    captureFmtLabel.setText(text, juce::dontSendNotification);

    juceBufferSize.setText("Juce Audio Buffer Size: " + std::to_string(audioProcessor.juceBufferSize.load()), juce::dontSendNotification);

    juceSampleRate.setText("Juce Audio Sample Rate: " + std::to_string(audioProcessor.juceSampleRate.load()), juce::dontSendNotification);
}