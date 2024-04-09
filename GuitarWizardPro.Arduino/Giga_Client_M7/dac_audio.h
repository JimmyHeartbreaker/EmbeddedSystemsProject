#pragma once


namespace Audio::D2AC
{
  void Setup(void (*copyDataToBuffer)(void*, int), int samplesPerChannel);
  void SendToOutput();
}
