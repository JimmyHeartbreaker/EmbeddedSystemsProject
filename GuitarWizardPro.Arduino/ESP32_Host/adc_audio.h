#pragma once
#include "shared.h"

namespace Audio::ADC
{
  const int BUFFER_SIZE = Shared::UNPACKED_PACKET_SIZE;
  uint16_t* PrimaryBuffer;
  void Setup(void (*onBufferFullEvent)(void* data, int length));
  void Teardown();
}

