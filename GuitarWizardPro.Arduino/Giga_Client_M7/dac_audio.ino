#include "dac_audio.h" 
#include "AudioDAC.h"

namespace Audio::D2AC
{
  AudioDAC dac0(A12);
  void (*copyDataToBufferFPtr)(uint16_t*);

  void Setup(void (*copyDataToBuffer)(uint16_t*), int samplesPerChannel)
  {  
     
    if (!dac0.init(AN_RESOLUTION_12,  samplesPerChannel)) {
        Serial.println("Failed to start DAC0 !");
        while (1);
    }
    copyDataToBufferFPtr = copyDataToBuffer;
    copyDataToBufferFPtr(dac0.getWriteBuffer());
    dac0.start();
    copyDataToBufferFPtr(dac0.getWriteBuffer());
    
  }
  void SendToOutput()
  {     
    if (dac0.writeRequired()) 
    { 
        copyDataToBufferFPtr(dac0.getWriteBuffer());
        dac0.writeCompleted();
    }
  }
}