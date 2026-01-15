/*
  ==============================================================================

    SharedRingBuffer.h
    Created: 15 Jan 2026 12:43:17pm
    Author:  Jiash

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SharedRingBuffer {
private:
    juce::AbstractFifo fifo;
    juce::AudioBuffer<float> buffer;
    int numChannels = 0;
    size_t capacity = 0;


public:
    SharedRingBuffer(size_t capacity, int numChannels);
    ~SharedRingBuffer();
    int pushFromCapClient(const float* data, int frameSize);
    int popToAudioProcessBlock(void* data, int frameSize);
    
};