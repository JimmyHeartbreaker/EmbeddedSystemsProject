#pragma once


namespace Audio::D2AC
{
  void Setup(void (*copyDataToBuffer)(uint16_t*), int samplesPerChannel);
  void SendToOutput();
}
