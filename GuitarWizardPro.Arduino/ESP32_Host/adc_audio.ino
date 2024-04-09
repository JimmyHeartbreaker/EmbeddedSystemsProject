#include <driver/adc.h>
#include "adc_audio.h"

namespace Audio::ADC
{
  //scheduling
  TaskHandle_t bufferFullTaskHandler;  
  hw_timer_t * adcTimer = NULL;
  SemaphoreHandle_t  semaOnBufferFull;
  
  //buffering
  void (*onBufferFullEvent)(void* data, int length);
  uint8_t* SecondaryBuffer; //this is the ptr to the buffer that we will fill up
  uint8_t* PrimaryBuffer;   //this is the ptr to the buffer that we will pass through 'onBufferFullEvent'  
  
  void onBufferFull(void *params)
  {
    while(true)
    {
      xSemaphoreTake( semaOnBufferFull,10000000 );  
      onBufferFullEvent(PrimaryBuffer,32);
    }
  }

  //runs 32k times per second. Is put into the   
  //place in Instructrum RAM for better performance 
  //https://docs.espressif.com/projects/esp-idf/en/stable/esp32s2/api-guides/memory-types.html
  void IRAM_ATTR onTimer32k() 
  {
    static int16_t sampleCount = 0;  //runs from 0 -> Shared::PACKED_SAMPLES_PER_PACKET repeated 
    static uint16_t xn1=0;
    //each sample is 12 bits. The smallest usable value in c is a byte or a short (2 bytes). 
    //Therefore if we want to maximize the space when transmitting data we want to pack these 12 bits into 3 bytes (2*12bits = 3 bytes)
    //we can divide up the samples into odd and even. The even samples will be split into 8/4 bits with the first 8 bits placed in the 1st byte
    //the last 4 bits will be placed in the upper 1/2 of the nexrt byte. The odd samples will also be split into 8/4 with the data placed in the remaining space
    //as below
    // FF
    // FE
    // EE
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
    adc1_config_width(ADC_WIDTH_BIT_12); //12 bits
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_2_5);    //0-1.25V
    analogRead(ADC1_CHANNEL_0);//must be called once at the start to initialize some driver stuff
    setCpuFrequencyMhz(240); //set cpu to 240mhz incase it isnt already set
    semaOnBufferFull = xSemaphoreCreateBinary();
    onBufferFullEvent = pOnBufferFullEvent;
    PrimaryBuffer =  (uint8_t*)malloc(Shared::PACKET_SIZE); //create buffers of 32 bytes - this is the maximum size the radio can send in 1 packet
    SecondaryBuffer = (uint8_t*)malloc(Shared::PACKET_SIZE);
   
    xTaskCreatePinnedToCore(onBufferFull, "Buffer Full Task", 8192, NULL, 1, &bufferFullTaskHandler,0);
    
    //the timers of the ESP32 are 80 MHZ. Using the CPU Divider of 2500 gives us 32k.
    adcTimer = timerBegin(3, 2499, true);
    timerAttachInterrupt(adcTimer, &onTimer32k, true);
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