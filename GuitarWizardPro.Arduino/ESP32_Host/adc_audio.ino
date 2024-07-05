#include <driver/adc.h>
#include "adc_audio.h"
#include "soc/sens_struct.h"

namespace Audio::ADC
{
  //scheduling
  TaskHandle_t bufferFullTaskHandler;  
  hw_timer_t * adcTimer = NULL;
  SemaphoreHandle_t  semaOnBufferFull;
  
  //buffering
  void (*onBufferFullEvent)(void* data, int length);
  uint16_t* SecondaryBuffer; //this is the ptr to the buffer that we will fill up
  uint16_t* PrimaryBuffer;   //this is the ptr to the buffer that we will pass through 'onBufferFullEvent'  
 
  void onBufferFull(void *params)
  {
    while(true)
    {
      xSemaphoreTake( semaOnBufferFull,10000000 );  
      onBufferFullEvent(PrimaryBuffer,32);      
    }
  }

  uint32_t adc_sample()
  {
    while (SENS.sar_slave_addr1.meas_status != 0);
    SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
    SENS.sar_meas1_ctrl2.meas1_start_sar = 1;
    while (SENS.sar_meas1_ctrl2.meas1_done_sar == 0);
    return SENS.sar_meas1_ctrl2.meas1_data_sar;

  }
  int adc_4x_sample()
  {
      return adc_sample() + adc_sample() + adc_sample() + adc_sample();
  }

  #define SENS_FORCE_XPD_AMP_PD  2 // Force power down
  void adc1_fsm_disable()
  {
    //channel is set in the  convert function
    SENS.sar_meas1_ctrl1.force_xpd_amp = SENS_FORCE_XPD_AMP_PD;
    //disable FSM, it's only used by the LNA.
    SENS.sar_amp_ctrl3.amp_rst_fb_fsm = 0;
    SENS.sar_amp_ctrl3.amp_short_ref_fsm = 0;
    SENS.sar_amp_ctrl3.amp_short_ref_gnd_fsm = 0;
    SENS.sar_amp_ctrl1.sar_amp_wait1 = 1;
    SENS.sar_amp_ctrl1.sar_amp_wait2 = 1;
    SENS.sar_amp_ctrl2.sar_amp_wait3 = 1;    
  }

  void adc_set_controller()
  {
    SENS.sar_meas1_mux.sar1_dig_force = false;      //RTC controller controls the ADC, not digital controller
    SENS.sar_meas1_ctrl2.meas1_start_force = true;  //RTC controller controls the ADC,not ulp coprocessor
    SENS.sar_meas1_ctrl2.sar1_en_pad_force = true;  //RTC controller controls the data port, not ulp coprocessor
    SENS.sar_hall_ctrl.xpd_hall_force = true;     // RTC controller controls the hall sensor power,not ulp coprocessor
    SENS.sar_hall_ctrl.hall_phase_force = true;   // RTC controller controls the hall sensor phase,not ulp coprocessor
  }

  //runs 32k times per second. Is put into the   
  //place in Instructrum RAM for better performance 
  //https://docs.espressif.com/projects/esp-idf/en/stable/esp32s2/api-guides/memory-types.html
  uint16_t prev=2047;
  void IRAM_ATTR onTimer32k() 
  {
    
    static int16_t sampleCount = 0;  //runs from 0 -> Shared::PACKET_SIZE/2 repeated 
    
    //we are going to sample 4 times to reduce noise. We will reduce the gain in the receiving side 
    SecondaryBuffer[sampleCount] =adc_4x_sample();
  
    sampleCount++;
    if (sampleCount >= Shared::PACKET_SIZE/2) 
    { 
      uint16_t* temp = PrimaryBuffer;
      PrimaryBuffer = SecondaryBuffer;
     
      xSemaphoreGiveFromISR(semaOnBufferFull, NULL);       
      sampleCount = 0;
      SecondaryBuffer =temp;           
    }
  }

  void Setup(void (*pOnBufferFullEvent)(void* data, int length))
  { 
     SENS.sar_meas1_ctrl2.sar1_en_pad = (1 << ADC1_CHANNEL_0); //only one channel is selected.       
    SENS.sar_meas1_mux.sar1_dig_force = 0;
     adc_power_acquire();  
    adc1_fsm_disable(); //currently the LNA is not open, close it by default
    adc_set_controller(  );
    adc1_config_width(ADC_WIDTH_BIT_12); //12 bits
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_2_5);    //0-1.25V

     //run once to make sure that things are in the correct state
    int value =analogRead(ADC1_CHANNEL_0);

          
    Serial.println(value);
    //run once to make sure that things are in the correct state
     value = adc1_get_raw(ADC1_CHANNEL_0);
    
    Serial.println(value);
    setCpuFrequencyMhz(240); //set cpu to 240mhz incase it isnt already set
    semaOnBufferFull = xSemaphoreCreateBinary();
    onBufferFullEvent = pOnBufferFullEvent;
    PrimaryBuffer =  (uint16_t*)malloc(Shared::PACKET_SIZE); //create buffers of 32 bytes - this is the maximum size the radio can send in 1 packet
    SecondaryBuffer = (uint16_t*)malloc(Shared::PACKET_SIZE);
   
    xTaskCreatePinnedToCore(onBufferFull, "Buffer Full Task", 8192, NULL, 1, &bufferFullTaskHandler,0);
 
     adcTimer = timerBegin(3, 2500, true);
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
    timerEnd(adcTimer);
    vTaskDelete(bufferFullTaskHandler);
    free(PrimaryBuffer);
    free(SecondaryBuffer);
    onBufferFullEvent = NULL;
    vSemaphoreDelete(semaOnBufferFull);    
  }
}