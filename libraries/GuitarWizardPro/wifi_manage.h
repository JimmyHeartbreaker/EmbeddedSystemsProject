#pragma once

#include "faults.h"
using namespace Faults;
namespace Wifi
{
  Fault* Host(char* ssid, char* password);
  Fault* Connect(char* ssid, char* passkey);
  void Disconnect();
 
}