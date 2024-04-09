#include "dac_audio.h"
#include "AudioDAC.h"
#include "Shared.h"

namespace Audio::D2AC 
{
  AudioDAC dac0(A12);
  void (*copyDataToBufferFPtr)(void*, int);

  uint8_t buffer[Shared::PACKET_SIZE];
  const int ZEROLINE = 2048;
  const int MAXAUDIO = 4095;

  //we must unpack out 2 samples in 3 byte structure
  void UnpackAudio() 
  {
    static bool hasAvg = false;
    static uint32_t avg;
    
    uint16_t* outBuf = dac0.getWriteBuffer();
    uint8_t* outBufPos = (uint8_t*)outBuf;

    uint8_t* endBuf = outBufPos + Shared::PACKED_SAMPLES_PER_PACKET*2;
    uint8_t* bufPos = buffer;
    
    bool even = true;
    int i = 0;
    static uint16_t prevRaw;
    static float yn1;
    
    static float w0=1;
    static float targetW0=1;
  
    //gradually adjust filtering as a sudden change in filtering is noise in itself
    w0 += (targetW0 - w0)/64;
  
    float totalRawDx;
    uint32_t total = 0;
    uint32_t totalRawMidPt = 0;

    while (outBufPos < endBuf) 
    {
      uint8_t* bp = bufPos + ((i * 3) >> 1);

      if (even) 
      {
        *outBufPos = *bp;
        *(outBufPos + 1) = ((*(bp + 1)) & 0xF0) >> 4;
      } 
      else 
      {
        *outBufPos = (*(bp + 1));
        *(outBufPos + 1) = (*bp) & 0x0F;
      }

      uint16_t* px = (uint16_t*)(outBufPos);
      uint16_t raw = *px;
      if (hasAvg) 
      {
        //correct any issue with offset. the 12 bit dac goes from 0-4095
        *px *= (2048.0 / avg);
      }
      //apply basic filtering
      float yn = (yn1  *w0) + ((*px)*(1-w0));
      *px = min(MAXAUDIO,(uint16_t)yn);
      yn1 = yn;

      //monitor activity by taking the absolute difference of the last sample and this sample
      float rawDx = raw - prevRaw;
      totalRawDx += abs(rawDx);
      prevRaw = raw;

      totalRawMidPt += prevRaw;
      total += abs(yn - ZEROLINE);
      i++;
      even = !even;
      outBufPos += 2;
    }

    float avgRawDx = totalRawDx / Shared::PACKED_SAMPLES_PER_PACKET;
    
    //if nothing is happening work out an average
    if (avgRawDx < 8 && (total/Shared::PACKED_SAMPLES_PER_PACKET < 10  || !hasAvg ) )
    {
      avg = totalRawMidPt / Shared::PACKED_SAMPLES_PER_PACKET;
      if(avg>1500 && avg < 2500)
      {
      hasAvg = true;
      }
    }     
    
    //set the filtering to be more aggressive when we are not playing and less aggressive when we are playing
    targetW0 = max(0.6,min(1,5.0/ avgRawDx));    
  }

  void Setup(void (*copyDataToBuffer)(void*,int), int samplesPerChannel) 
  {
    copyDataToBufferFPtr = copyDataToBuffer;

    //init the DAC0 on the DMA controller
    if (!dac0.init(AN_RESOLUTION_12, samplesPerChannel)) 
    {
      Serial.println("Failed to start DAC0 !");   
    }
    
    //get some data just before we kick it off
    copyDataToBufferFPtr(buffer,Shared::PACKET_SIZE);
    //unpack it
    UnpackAudio();
    //start
    dac0.start();
    //we already know that the 2nd 1/2 of the circular buffer must be filled up before the 1st 1/2 has completed so lets get on with it
    copyDataToBufferFPtr(buffer,Shared::PACKET_SIZE);
    UnpackAudio();
  }

  void WriteAudioToDMABuffer() 
  {
    //wait for next buffer to come through
    copyDataToBufferFPtr(buffer,Shared::PACKET_SIZE);

    //as soon as DMA interrupt for the next block of data occurs we can then copy that to the buffer with a bit of processing in the middle
    while (!dac0.writeRequired()) {}
    UnpackAudio();
    dac0.writeCompleted();
  }
}