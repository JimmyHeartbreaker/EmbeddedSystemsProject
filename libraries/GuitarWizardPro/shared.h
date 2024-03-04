#pragma once

namespace Shared
{
    
    const char APPDATA_RECIEVE='a';
    struct AppData
    {
        int16_t* PrimaryAudioBuffer;
        int PrimaryAudioBufferLength;
        bool dataAvailable=false;
    };
}