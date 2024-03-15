#pragma once
#include "faults.h"
#include "lwip/priv/tcpip_priv.h"


namespace Audio::UDP
{
  void Setup();
  Faults::Fault* Send(pbuf* buf);
}
