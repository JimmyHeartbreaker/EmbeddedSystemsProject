#pragma once
#include "shared.h"



#include "lwip/priv/tcpip_priv.h"

namespace Audio::ADC
{
  void Setup(void (*onBufferFullEvent)(void* data, int length));
  void Teardown();
}

