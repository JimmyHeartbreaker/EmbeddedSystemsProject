#pragma once


namespace Audio::ADC
{
  const int BUFFER_SIZE = 256;
  int16_t* PrimaryBuffer;
  void Setup(void (*onBufferFullEvent)());
  void Teardown();
}

