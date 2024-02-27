#pragma once
#include "faults.h"


namespace Audio::UDP
{
  Faults::Fault* Send(uint8_t* data, int length);
}
