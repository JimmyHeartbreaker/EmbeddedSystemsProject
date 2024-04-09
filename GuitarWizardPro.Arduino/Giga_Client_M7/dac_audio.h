#pragma once


namespace Audio::D2AC
{
  //setup the Circular dma buffer 
  void Setup(void (*readAudio)(void*, int), int samplesPerChannel);
  //must be called continuously
  void WriteAudioToDMABuffer();
}
