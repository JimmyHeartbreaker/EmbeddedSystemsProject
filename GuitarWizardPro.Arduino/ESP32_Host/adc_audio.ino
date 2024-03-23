#include <dummy.h>

#include <driver/adc.h>
#include "adc_audio.h"
#include <math.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

namespace Audio::ADC
{

  RF24 radio(9, 10,20000000); 


  portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;
  TaskHandle_t bufferFullTaskHandler;
  
  hw_timer_t * adcTimer = NULL;
  uint8_t* SecondaryBuffer;
  int16_t sampleCount = 0;
  void (*onBufferFullEvent)();
  SemaphoreHandle_t  semaOnBufferFull; 
 
  void swapBuffers()
  {
      uint16_t* temp = PrimaryBuffer;
      PrimaryBuffer = (uint16_t*)SecondaryBuffer;
      SecondaryBuffer = (uint8_t*)temp;     
  }
int writeCount=0;
long int mills;
  void onBufferFull(void *params)

  {
    while(true)
    {
      xSemaphoreTake( semaOnBufferFull,pdMS_TO_TICKS(100000) );  
    
      // writeCount++;
      // long int t = millis();
      // if(t-mills > 1000)
      // {
      //   Serial.println(writeCount);
      //   writeCount = 0;
      //   mills = t;
      // }
        radio.writeFast(PrimaryBuffer, 32);
      
    }
  }
  int i=0;
  void IRAM_ATTR onTimer() 
  {
    
   // Serial.println("6");
    portENTER_CRITICAL_ISR(&timerMux);      
   
   // Serial.println("7");
  //  if((i++%32000)==0)
  //  {
  //   Serial.println(millis());
  //  }
    uint16_t bufPos = (sampleCount * 3) / 2;
   if(sampleCount %2 == 0)
   {
    //even    

    uint16_t value = ((uint16_t)adc1_get_raw(ADC1_CHANNEL_0));
     //;
    SecondaryBuffer[bufPos] = (uint8_t)value;//store the first byte    
    SecondaryBuffer[bufPos+1] =(uint8_t)((value & 0x0F00) >> 4);//store the remaining 4 bits in the upper 1/2 of the byte
   // Serial.println(((uint16_t*)SecondaryBuffer)[bufPos]);
   }
   else
   {
    uint16_t value = adc1_get_raw(ADC1_CHANNEL_0);
    SecondaryBuffer[bufPos] |= (uint8_t)((value & 0x0F00 )>>8);//store the remaining 4 bits in the lower 1/2 of the byte
    SecondaryBuffer[bufPos+1] =(uint8_t)( value);//store the first byte    
   }
    sampleCount++;
   if (sampleCount >= 21) 
   { 
     swapBuffers();
     sampleCount = 0;
     xSemaphoreGiveFromISR(semaOnBufferFull, NULL);        
   }

    portEXIT_CRITICAL_ISR(&timerMux);
  }

  void IRAM_ATTR Setup(void (*pOnBufferFullEvent)())
  {  
     adc1_config_width(ADC_WIDTH_BIT_12);
     adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_2_5);    
     analogRead(ADC1_CHANNEL_0);
     setCpuFrequencyMhz(240);
     semaOnBufferFull = xSemaphoreCreateBinary();
    // onBufferFullEvent = pOnBufferFullEvent;
    PrimaryBuffer =  (uint16_t*)malloc(32);//pbuf_alloc(PBUF_TRANSPORT, BUFFER_SIZE*2, PBUF_RAM);
    SecondaryBuffer = (uint8_t*)malloc(32);//pbuf_alloc(PBUF_TRANSPORT, BUFFER_SIZE*2, PBUF_RAM);
   
    xTaskCreatePinnedToCore(onBufferFull, "Buffer Full Task", 8192, NULL, 1, &bufferFullTaskHandler,0);
    
    pinMode(D10,OUTPUT);
    Serial.println("1");  
    while(!radio.begin())
    {
        Serial.println("not successfull");
        delay(1000);
    }
    
    radio.setPALevel(RF24_PA_MIN);
    radio.setRetries(0,0);
    radio.setDataRate(RF24_2MBPS );
    radio.setAutoAck(false);
   radio.setChannel(124);
   radio.openWritingPipe(0xF0F0F0F0F0);   
  radio.openReadingPipe(1,0xF0F0F0F0F1);

  radio.stopListening();
  
  radio.printPrettyDetails();
    while(!radio.isChipConnected())
    {
        Serial.println("chip not connected1");
        delay(100);
    }

    //radio.stopListening();
   adcTimer = timerBegin(3, 2, true);
    timerAttachInterrupt(adcTimer, &onTimer, true);
    timerAlarmWrite(adcTimer, 1247, true); 
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