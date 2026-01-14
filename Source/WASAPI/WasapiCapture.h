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
#include <avrt.h>
#include <stdio.h>
#include <atomic>

class WasapiCapture {
private:
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;
    IAudioClient* audioClient = nullptr;
    IAudioCaptureClient* captureClient = nullptr;
    WAVEFORMATEX* mixFmt = nullptr;
    std::atomic<HRESULT>& m_isRunning;

public:
    WasapiCapture(std::atomic<HRESULT>& isRunning);
    ~WasapiCapture();
    void initialize();
    void startCapture();
    void stopCapture();
    void begin(); // Private member variables and methods for WASAPI capture
                  // implementation
};