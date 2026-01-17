/*
  ==============================================================================

    SharedRingBuffer.cpp
    Created: 15 Jan 2026 12:43:17pm
    Author:  Jiash

  ==============================================================================
*/

#include "SharedRingBuffer.h"

SharedRingBuffer::SharedRingBuffer(size_t capacity, int numChannels)
    : fifo((int)capacity)
    , buffer(numChannels, (int)capacity)
{
    this->numChannels = numChannels;
    this->capacity = capacity;
    buffer.clear();
}

SharedRingBuffer::~SharedRingBuffer()
{
}

int SharedRingBuffer::pushFromCapClient(const float* data, int frameSize)
{
    if (frameSize > capacity) {
        data += (frameSize - capacity) * numChannels;
        frameSize = (int)capacity;
    }

    int start1, size1, start2, size2;
    fifo.prepareToWrite(frameSize, start1, size1, start2, size2);

    int needDrop = frameSize - (size1 + size2);
    if (needDrop > 0)
        fifo.finishedRead(std::min(needDrop, fifo.getNumReady()));

    fifo.prepareToWrite(frameSize, start1, size1, start2, size2);

    for (int ch = 0; ch < numChannels; ++ch) {
        float* writePtr = buffer.getWritePointer(ch);
        for (int i = 0; i < size1; ++i) {
            writePtr[start1 + i] = data[i * numChannels + ch];
        }
        for (int i = 0; i < size2; ++i) {
            writePtr[start2 + i] = data[(size1 + i) * numChannels + ch];
        }
    }

    fifo.finishedWrite(size1 + size2);

    return size1 + size2;
}

int SharedRingBuffer::copyToAudioProcessBlock(juce::AudioBuffer<float>& dst, int frameSize)
{
    const int dstChannels = dst.getNumChannels();
    const int needFrames = frameSize;

    const int canRead = fifo.getNumReady();
    const int toRead = std::min(needFrames, canRead);

    int start1, size1, start2, size2;
    fifo.prepareToRead(toRead, start1, size1, start2, size2);

    for (int ch = 0; ch < dstChannels; ++ch) {
        dst.clear(ch, 0, dst.getNumSamples());
    }

    const int copyCh = std::min(dstChannels, numChannels);

    for (int ch = 0; ch < copyCh; ++ch) {
        const float* readPtr = buffer.getReadPointer(ch);
        float* writePtr = dst.getWritePointer(ch);
        if (size1 > 0) {
            std::memcpy(writePtr, readPtr + start1, size1 * sizeof(float));
        }
        if (size2 > 0) {
            std::memcpy(writePtr + size1, readPtr + start2, size2 * sizeof(float));
        }
    }

    //fifo.finishedRead(size1 + size2);
    return size1 + size2;
}

int SharedRingBuffer::popToAudioProcessBlock(juce::AudioBuffer<float>& dst, int frameSize)
{
    const int dstChannels = dst.getNumChannels();
    const int needFrames = frameSize;

    const int canRead = fifo.getNumReady();
    const int toRead = std::min(needFrames, canRead);

    int start1, size1, start2, size2;
    fifo.prepareToRead(toRead, start1, size1, start2, size2);

    for (int ch = 0; ch < dstChannels; ++ch) {
        dst.clear(ch, 0, dst.getNumSamples());
    }

    const int copyCh = std::min(dstChannels, numChannels);

    for (int ch = 0; ch < copyCh; ++ch) {
        const float* readPtr = buffer.getReadPointer(ch);
        float* writePtr = dst.getWritePointer(ch);
        if (size1 > 0) {
            std::memcpy(writePtr, readPtr + start1, size1 * sizeof(float));
        }
        if (size2 > 0) {
            std::memcpy(writePtr + size1, readPtr + start2, size2 * sizeof(float));
        }
    }

    fifo.finishedRead(size1 + size2);
    return size1 + size2;
}