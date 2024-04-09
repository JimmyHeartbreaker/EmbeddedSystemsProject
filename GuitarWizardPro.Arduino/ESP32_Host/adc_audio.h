#pragma once
#include "shared.h"



#include "lwip/priv/tcpip_priv.h"

namespace Audio::ADC
{
  //Setup the adc to receive data and forward that data to the function ptr 'onBufferFullEvent'
  void Setup(void (*onBufferFullEvent)(void* data, int length));
  void Teardown();
}

