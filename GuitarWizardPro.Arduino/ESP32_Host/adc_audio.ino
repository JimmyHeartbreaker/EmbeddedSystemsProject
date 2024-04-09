#include <driver/adc.h>
#include "adc_audio.h"

namespace Audio::ADC
{
  portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;
  TaskHandle_t bufferFullTaskHandler;
  
  hw_timer_t * adcTimer = NULL;
  uint8_t* SecondaryBuffer;
  uint8_t* PrimaryBuffer;
 
  int16_t sampleCount = 0;
  void (*onBufferFullEvent)(void* data, int length);
  SemaphoreHandle_t  semaOnBufferFull; 
 
  void IRAM_ATTR onBufferFull(void *params)
  {
    while(true)
    {
      xSemaphoreTake( semaOnBufferFull,10000000 );  
      onBufferFullEvent(PrimaryBuffer,32);
    }
  }
  void IRAM_ATTR onTimer() 
  {
    static uint16_t xn1=0;
    uint16_t bufPos =  (sampleCount * 3) >> 1;

    uint16_t xn = adc1_get_raw(ADC1_CHANNEL_0);
   
    uint16_t value = (xn1>>1) + (xn>>1);
    
    xn1 =xn;
    if(sampleCount %2 == 0)
    {
      SecondaryBuffer[bufPos] = (uint8_t)value;//store the first byte    
      SecondaryBuffer[bufPos+1] =(uint8_t)((value & 0x0F00) >> 4);//store the remaining 4 bits in the upper 1/2 of the byte
    }
    else
    {
      SecondaryBuffer[bufPos] |= (uint8_t)((value & 0x0F00 )>>8);//store the remaining 4 bits in the lower 1/2 of the byte
      SecondaryBuffer[bufPos+1] =(uint8_t)( value);//store the first byte    
    }
    
    sampleCount++;
    if (sampleCount >= Shared::PACKED_SAMPLES_PER_PACKET) 
    { 
      uint8_t* temp = PrimaryBuffer;
      PrimaryBuffer = SecondaryBuffer;
     
      xSemaphoreGiveFromISR(semaOnBufferFull, NULL);  
     
      sampleCount = 0;
      SecondaryBuffer =temp;           
    }
  }

  void Setup(void (*pOnBufferFullEvent)(void* data, int length))
  {  
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_2_5);    
    analogRead(ADC1_CHANNEL_0);
    setCpuFrequencyMhz(240);
    semaOnBufferFull = xSemaphoreCreateBinary();
    onBufferFullEvent = pOnBufferFullEvent;
    PrimaryBuffer =  (uint8_t*)malloc(Shared::PACKET_SIZE);
    SecondaryBuffer = (uint8_t*)malloc(Shared::PACKET_SIZE);
   
    xTaskCreatePinnedToCore(onBufferFull, "Buffer Full Task", 8192, NULL, 1, &bufferFullTaskHandler,0);
    
    adcTimer = timerBegin(3, 2499, true);
    timerAttachInterrupt(adcTimer, &onTimer, true);
    timerAlarmWrite(adcTimer, 1, true); 
    timerAlarmEnable(adcTimer);
  }

  void Teardown()
  {
    timerStop(adcTimer);
    timerAlarmDisable(adcTimer);
    timerDetachInterrupt(adcTimer);
    timerEnd(adcTimer);
    vTaskDelete(bufferFullTaskHandler);
    free(PrimaryBuffer);
    free(SecondaryBuffer);
    onBufferFullEvent = NULL;
    vSemaphoreDelete(semaOnBufferFull);    
  }
}