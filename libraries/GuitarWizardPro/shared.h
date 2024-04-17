#pragma once

namespace Shared
{
    const int PACKED_SAMPLES_PER_PACKET = 21;
    const int PACKET_SIZE = 32;
    const char APPDATA_RECIEVE='a';
    struct AppData
    {
        bool dataAvailable;
        uint8_t* AudioBuffer;
        int x;
    };
}
