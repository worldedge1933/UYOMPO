/*
  ==============================================================================

        WasapiCapture.h
        Created: 13 Jan 2026 11:31:11pm
        Author:  Jiash

  ==============================================================================
*/

#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Audioclient.h>
#include <Mmdeviceapi.h>
#include <Windows.h>
#include <atomic>
#include <avrt.h>
#include <stdio.h>
#include <thread>

#include "../SharedRingBuffer/SharedRingBuffer.h"

class WasapiCapture {
private:
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;
    IAudioClient* audioClient = nullptr;
    IAudioCaptureClient* captureClient = nullptr;

    std::atomic<HRESULT>& m_isRunning;
    HANDLE hCaptureEvent = nullptr;
    HANDLE hTask = nullptr;
    BYTE* pData = nullptr;
    UINT32 numFramesToRead = 0;
    DWORD silenceFlag = 0;

    SharedRingBuffer* m_ringBuffer = nullptr;

public:
    WasapiCapture(std::atomic<HRESULT>& isRunning, SharedRingBuffer* m_ringBuffer);
    ~WasapiCapture();
    void initialize();
    void startCapture();
    void stopCapture();
    WAVEFORMATEX* mixFmt = nullptr;
    uint32_t capSampleRate;
    uint16_t capChannels;

    REFERENCE_TIME bufferDuration = 10 * 1000 * 10;
    std::atomic<int> failReason { 0 };

    std::atomic<FLOAT> capturedVolume = { 1 };

    std::atomic<int> capturedFrames = { 0 };
};