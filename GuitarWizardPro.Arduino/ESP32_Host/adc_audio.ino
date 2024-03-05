#include <dummy.h>

#include <driver/adc.h>
#include "adc_audio.h"


namespace Audio::ADC
{

  portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;
  TaskHandle_t bufferFullTaskHandler;
  hw_timer_t * adcTimer = NULL;
  int16_t* SecondaryBuffer;
  int16_t writeBufPos = 0;
  void (*onBufferFullEvent)();
  SemaphoreHandle_t  semaOnBufferFull; 
 
  void swapBuffers()
  {
    int16_t* temp = SecondaryBuffer;
    SecondaryBuffer = PrimaryBuffer;
    PrimaryBuffer = temp;
  }

  void onBufferFull(void *params)
  {
    while(true)
    {
      xSemaphoreTake( semaOnBufferFull,pdMS_TO_TICKS(100000) );  
      onBufferFullEvent();  
    }
  }

  void IRAM_ATTR onTimer() 
  {
    portENTER_CRITICAL_ISR(&timerMux);       
    SecondaryBuffer[writeBufPos++] = adc1_get_raw(ADC1_CHANNEL_0);
  
    if (writeBufPos >= BUFFER_SIZE) 
    { 
      writeBufPos = 0;
      swapBuffers();
      xSemaphoreGiveFromISR(semaOnBufferFull, NULL);
    }

    portEXIT_CRITICAL_ISR(&timerMux);
  }

  void Setup(void (*pOnBufferFullEvent)())
  {  
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_2_5);    
    analogRead(ADC1_CHANNEL_0);
    setCpuFrequencyMhz(240);
    semaOnBufferFull = xSemaphoreCreateBinary();
    onBufferFullEvent = pOnBufferFullEvent;
    PrimaryBuffer = (int16_t*)malloc(BUFFER_SIZE*2);
    SecondaryBuffer =  (int16_t*)malloc(BUFFER_SIZE*2);
    xTaskCreatePinnedToCore(onBufferFull, "Handler Task", 8192, NULL, 1, &bufferFullTaskHandler,0);
    adcTimer = timerBegin(3, 20, true);
    timerAttachInterrupt(adcTimer, &onTimer, true);
    timerAlarmWrite(adcTimer, 125, true); 
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