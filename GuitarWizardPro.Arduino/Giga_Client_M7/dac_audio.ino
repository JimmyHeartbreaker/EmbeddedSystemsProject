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
      uint8_t* outBuf = (uint8_t*)dac0.getWriteBuffer();
      
        uint8_t* endBuf = outBuf+42;
        uint8_t* bufPos = buffer;
      //  Serial.println(((uint8_t*)bufPos)[0]);
        bool even=true;
        int i=0;
        static uint16_t xn1;
        static uint16_t prev;
        static uint16_t yn1;

        float totalDx;
        uint32_t total=0;
        uint32_t totalMidPt=0;
        
        static int noSound;
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
          uint16_t* px = (uint16_t*)(outBuf);
          if(hasAvg)
          {
            uint16_t  nv= min(4095, *px *  (2048.0/avg));
            
            *px  = nv;
          }
            uint16_t xn = *px;
            uint16_t yn = (yn1>>1) + (xn>>2) + (xn1>>2);
            *px = yn;
            yn1=yn;
            xn1=xn;

          float dx = (*px)-prev;
                    
          
          totalDx += abs(dx);
          prev = *px;

          totalMidPt += prev;
          total+= abs(prev-2048);
        //  *() +=400;
        //  Serial.println(value);
          i++;
          even=!even;
          outBuf+=2;
        }
        
        static int offCount = 0;
        static bool isOff =false;
        float avgDx = totalDx/ 21;
        if(abs(avgDx) < 4.5)
        {
          //calculatingAverage = false;
          
           noSound++;
        }
        else if(noSound > 15)
        {
           if(abs(avgDx) > 15)
           {
            noSound = 0;
            isOff=false;
            offCount = 0;
           }
        }
        else
        {  
          
          noSound = 0;
        }
        if(noSound > 15)
        {
          if(total/21 < 15)
          {
            offCount++;
            if(offCount>5)
            {
              isOff =true;
            }
          
          }
          else
          {
            offCount--;
          }
          if(!hasAvg  || isOff)
          {
             uint16_t* outBuf2 = (uint16_t*)dac0.getWriteBuffer();;
           for(int i =0;i<21;i++)
           {
              outBuf2[i] = 2048;
           }
          }
           if(!hasAvg)
           {
            avg = totalMidPt/21;
            hasAvg=true;
            
           // Serial.println(avg);    
           }
           //Serial.println(total/21);           
        }

        
        //Serial.println(avgDx);
        
  }
  void Setup(void (*copyDataToBuffer)(uint8_t*), int samplesPerChannel)
  {  
     
    if (!dac0.init(AN_RESOLUTION_12,  samplesPerChannel)) {
        Serial.println("Failed to start DAC0 !");
        while (1);
    }
    copyDataToBufferFPtr = copyDataToBuffer;
    
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
        //Serial.println(dac0.getWriteBuffer()[0]);
    dac0.writeCompleted();
        
  }
}