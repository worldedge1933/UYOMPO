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

class WasapiCapture {
private:
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;
    IAudioClient* audioClient = nullptr;
    IAudioCaptureClient* captureClient = nullptr;
    WAVEFORMATEX* mixFmt = nullptr;
    std::atomic<HRESULT>& m_isRunning;
    HANDLE hCaptureEvent = nullptr;
    HANDLE hTask = nullptr;

public:
    WasapiCapture(std::atomic<HRESULT>& isRunning);
    ~WasapiCapture();
    void initialize();
    void startCapture();
    void stopCapture();

    uint32_t capSampleRate;
    uint16_t capChannels;

    REFERENCE_TIME bufferDuration = 10 * 100000;
    std::atomic<int> failReason { 0 };
};