#pragma once

#include <SPI.h>

namespace  Radio::Transceive
{
  void SetupForTransmit(SPIClass* spi);
  void SetupForReceive(SPIClass* spi);
 void SendData(void* data, int length);  
 void ReceiveData(void* buffer, int length);

}

