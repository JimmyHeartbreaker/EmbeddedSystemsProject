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
  int writeCount=0;
  long int mills;
  uint8_t buffer[32];
  uint8_t buffer2[42];
  void SendToOutput()
  {     
   
    
        
      writeCount++;
      long int t = millis();
      if(t-mills > 1000)
      {
        Serial.println(writeCount);
        writeCount = 0;
        mills = t;
      }
        copyDataToBufferFPtr((uint16_t*)buffer);
         while (!dac0.writeRequired())
    {
       // Serial.println("waiting");
      } 
        uint8_t* outBuf = (uint8_t*)dac0.getWriteBuffer();
        uint8_t* endBuf = outBuf+42;
        uint8_t* bufPos = buffer;
        bool even=true;
        int i=0;
        while(outBuf<endBuf)
        {
          
          uint8_t* bp = bufPos + ((i * 3) >> 1);
        
          if(even)
          {
            *outBuf = *bp;
            *(outBuf+1) = ((*(bp+1)) & 0xF0) >> 4;
          }
          else
          {
            *outBuf  = (*(bp+1));
             *(outBuf+1) = (*bp) & 0x0F;
          }
          i++;
          even=!even;
          outBuf+=2;
        }
    //    delay(1);
    dac0.writeCompleted();
        
  }
}