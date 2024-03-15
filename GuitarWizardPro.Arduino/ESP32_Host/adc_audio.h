#pragma once
#include "shared.h"



#include "lwip/priv/tcpip_priv.h"

namespace Audio::ADC
{
  const int BUFFER_SIZE = Shared::SAMPLES_PER_PACKET;
  pbuf* PrimaryBuffer;
  void Setup(void (*onBufferFullEvent)());
  void Teardown();
}

