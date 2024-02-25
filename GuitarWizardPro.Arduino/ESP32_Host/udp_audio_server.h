#pragma once



namespace Audio::UDP
{
  bool StartServer();
  void SendAudio(uint8_t* data, int length);

}
