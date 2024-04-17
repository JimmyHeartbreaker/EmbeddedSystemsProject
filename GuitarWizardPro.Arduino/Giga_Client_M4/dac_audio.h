#pragma once


namespace Audio::D2AC
{
  
  //setup the Circular dma buffer 
  void Setup(int samplesPerChannel);
  //must be called continuously
  void UnpackAndStream(uint8_t* buffer);
}
