#include "dac_audio.h"
#include "AudioDAC.h"

namespace Audio::D2AC 
{
  AudioDAC dac0(A12);
  void (*copyDataToBufferFPtr)(uint8_t*);

  uint8_t buffer[32];

  void UnpackAudio() 
  {
    static bool hasAvg = false;
    static uint32_t avg;
    
    uint16_t* outBuf = dac0.getWriteBuffer();
    uint8_t* outBufPos = (uint8_t*)outBuf;

    uint8_t* endBuf = outBufPos + 42;
    uint8_t* bufPos = buffer;
    
    bool even = true;
    int i = 0;
    static uint16_t prevRaw;
    static float yn1;
    
    static float w0=1;
    static float targetW0=1;
  
    w0 += (targetW0 - w0)/64;
  
    float totalRawDx;
    uint32_t total = 0;
    uint32_t totalRawMidPt = 0;

    while (outBufPos < endBuf) {

      uint8_t* bp = bufPos + ((i * 3) >> 1);

      if (even) {

        *outBufPos = *bp;
        *(outBufPos + 1) = ((*(bp + 1)) & 0xF0) >> 4;
      } else {
        *outBufPos = (*(bp + 1));
        *(outBufPos + 1) = (*bp) & 0x0F;
      }

      uint16_t* px = (uint16_t*)(outBufPos);
      uint16_t raw = *px;
      if (hasAvg) 
      {
        *px *= (2048.0 / avg);
      }
      float yn = (yn1  *w0) + ((*px)*(1-w0));
      *px = min(4095,(uint16_t)yn);
      yn1 = yn;

      float rawDx = raw - prevRaw;
      totalRawDx += abs(rawDx);
      prevRaw = raw;

      totalRawMidPt += prevRaw;
      total += abs(yn - 2048);
      i++;
      even = !even;
      outBufPos += 2;
    }

    float avgRawDx = totalRawDx / 21;
    
    if (avgRawDx < 8 && (total/21 < 10  || !hasAvg ) )
    {
      avg = totalRawMidPt / 21;
      if(avg>1500 && avg < 2500)
      {
      hasAvg = true;
      }
    }     
    
    targetW0 = max(0.6,min(1,5.0/ avgRawDx));
    
  }

  void Setup(void (*copyDataToBuffer)(uint8_t*), int samplesPerChannel) {
    copyDataToBufferFPtr = copyDataToBuffer;

    if (!dac0.init(AN_RESOLUTION_12, samplesPerChannel)) {
      Serial.println("Failed to start DAC0 !");   
    }
    
    copyDataToBufferFPtr(buffer);
    UnpackAudio();
    dac0.start();
    copyDataToBufferFPtr(buffer);
    UnpackAudio();
  }

  void SendToOutput() 
  {
    copyDataToBufferFPtr(buffer);

    while (!dac0.writeRequired()) {
    }
    UnpackAudio();
    dac0.writeCompleted();
  }
}