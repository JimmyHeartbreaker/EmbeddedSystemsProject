#pragma once


namespace Audio::D2AC
{
  void Setup(void (*copyDataToBuffer)(uint8_t*), int samplesPerChannel);
  void SendToOutput();
}
