#pragma once



namespace Audio::UDP
{
  void Setup();
  int Receive(void* buffer,int length);
  void Send(void* buffer, int length);
}
