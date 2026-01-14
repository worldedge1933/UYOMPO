/*
  ==============================================================================

    WasapiCapture.cpp
    Created: 13 Jan 2026 11:31:11pm
    Author:  Jiash

  ==============================================================================
*/

#include "WasapiCapture.h"

WasapiCapture::WasapiCapture(std::atomic<HRESULT>& isRunning)
    : m_isRunning(isRunning)
{
}

WasapiCapture::~WasapiCapture()
{   
    if (hTask)
    {
        AvRevertMmThreadCharacteristics(hTask);
    }
    if (hCaptureEvent) {
        CloseHandle(hCaptureEvent);
    }
    if (this->mixFmt) {
        CoTaskMemFree(this->mixFmt);
    }
    if (captureClient)
        captureClient->Release();
    if (audioClient)
        audioClient->Release();
    if (device)
        device->Release();
    if (enumerator)
        enumerator->Release();




}

void WasapiCapture::initialize()
{

    m_isRunning.store(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    if (FAILED(m_isRunning.load())) {
        failReason.store(1);
        return;
    }

    m_isRunning.store(CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (void**)&enumerator));

    if (FAILED(m_isRunning.load())) {
        failReason.store(2);
        return;
    }

    m_isRunning.store(enumerator->GetDefaultAudioEndpoint(eCapture, eMultimedia, &device));

    if (FAILED(m_isRunning.load())) {
        failReason.store(3);
        return;
    }

    m_isRunning.store(device->Activate(
        __uuidof(IAudioClient),
        CLSCTX_ALL,
        NULL,
        (void**)&audioClient));

    if (FAILED(m_isRunning.load())) {
        failReason.store(4);
        return;
    }

    m_isRunning.store(audioClient->GetMixFormat(&mixFmt));

    if (FAILED(m_isRunning.load())) {
        failReason.store(5);
        return;
    }

    capChannels = mixFmt->nChannels;
    capSampleRate = mixFmt->nSamplesPerSec;

    m_isRunning.store(audioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        bufferDuration,
        0,
        mixFmt,
        nullptr));

    if (FAILED(m_isRunning.load())) {
        failReason.store(6);
        return;
    }

    hCaptureEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    m_isRunning.store(audioClient->SetEventHandle(hCaptureEvent));

    if (FAILED(m_isRunning.load())) {
        failReason.store(7);
        return;
    }

    m_isRunning.store(audioClient->GetService(
        __uuidof(IAudioCaptureClient),
        (void**)&captureClient));

    if (FAILED(m_isRunning.load())) {
        failReason.store(8);
        return;
    }

    DWORD taskIndex = 0;
    hTask = AvSetMmThreadCharacteristics(TEXT("Pro Audio"), &taskIndex);

    m_isRunning.store(audioClient->Start());

    if (FAILED(m_isRunning.load())) {
        failReason.store(9);
        return;
    }

    
}

void WasapiCapture::startCapture()
{
    if (SUCCEEDED(m_isRunning.load())) {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}