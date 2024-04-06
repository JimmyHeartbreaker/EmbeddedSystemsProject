#pragma once

namespace  Radio::Transceive
{
  void Setup();
 void IRAM_ATTR SendData(void* data, int length);
}

