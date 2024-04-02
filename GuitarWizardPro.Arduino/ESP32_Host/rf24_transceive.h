#pragma once

namespace  RF24::Transceive
{
  void Setup();
 void IRAM_ATTR SendData(void* data, int length);
}

