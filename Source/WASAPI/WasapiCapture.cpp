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
    m_isRunning.store(CoInitializeEx(NULL, COINIT_MULTITHREADED));
}