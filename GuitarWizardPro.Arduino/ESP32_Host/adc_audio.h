#pragma once
#include "shared.h"



#include "lwip/priv/tcpip_priv.h"

namespace Audio::ADC
{
  const int BUFFER_SIZE = Shared::UNPACKED_PACKET_SIZE;
  uint8_t* PrimaryBuffer;
  void Setup(void (*onBufferFullEvent)(void* data, int length));
  void Teardown();
}

