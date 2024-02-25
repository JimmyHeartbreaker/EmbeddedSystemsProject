#pragma once


namespace Audio::ADC
{
  const int BUFFER_SIZE = 64;
  int16_t* PrimaryBuffer;  
  bool DoRead=false;
  void Setup(void (*onBufferFullEvent)());
}

