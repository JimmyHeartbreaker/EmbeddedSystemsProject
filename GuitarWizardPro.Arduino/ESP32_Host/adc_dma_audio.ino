/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/adc.h"


namespace Audio::ADC::DMA
{

  #define TIMES              128
  #define GET_UNIT(x)        ((x>>3) & 0x1)

  #define ADC_RESULT_BYTE     4
  #define ADC_CONV_LIMIT_EN   0
  #define ADC_CONV_MODE       ADC_CONV_SINGLE_UNIT_1
  #define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE2


  //scheduling
  TaskHandle_t bufferFullTaskHandler;  
  SemaphoreHandle_t  semaOnBufferFull;
  
  //buffering
  void (*onBufferFullEvent)(void* data, int length);
  uint32_t* SecondaryBuffer; 
  uint32_t* PrimaryBuffer;  
 


  static uint16_t adc1_chan_mask = BIT(2) | BIT(3);
  
  int count;
  int mills;
  void onBufferFull(void *params)
  {
    uint16_t buffer[16];
    while(true)
    {
      xSemaphoreTake( semaOnBufferFull,10000000 );  
      
         int t= millis();
      count++;
      if(t - mills > 1000)
      {
        mills =  millis();
        Serial.println(count);
        count = 0;
     }

      for (int i = 0; i < 16; i ++) 
      {
          adc_digi_output_data_t *p = (adc_digi_output_data_t*)&PrimaryBuffer[i];
          buffer[i] = p->type2.data;    
      }
      onBufferFullEvent(PrimaryBuffer,32);
      
    }
  }


  static void continuous_adc_init(uint16_t adc1_chan_mask)
  {
      adc_digi_init_config_t adc_dma_config = {
          .max_store_buf_size = 512,
          .conv_num_each_intr = TIMES,
          .adc1_chan_mask = adc1_chan_mask,
      };
      ESP_ERROR_CHECK(adc_digi_initialize(&adc_dma_config));

      adc_digi_configuration_t dig_cfg = {
          .conv_limit_en = ADC_CONV_LIMIT_EN,
          .conv_limit_num = 100,
          .sample_freq_hz = 32020,
          .conv_mode = ADC_CONV_MODE,
          .format = ADC_OUTPUT_TYPE,
      };

      adc_digi_pattern_config_t adc_pattern;
      dig_cfg.pattern_num = 0;
    //  for (int i = 0; i < channel_num; i++) {
          uint8_t unit = GET_UNIT(ADC1_CHANNEL_0);
          uint8_t ch = ADC1_CHANNEL_0 & 0x7;
          adc_pattern.atten = ADC_ATTEN_DB_2_5;
          adc_pattern.channel = ch;
          adc_pattern.unit = unit;
          adc_pattern.bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;

          ESP_LOGI(TAG, "adc_pattern[%d].atten is :%x", i, adc_pattern.atten);
          ESP_LOGI(TAG, "adc_pattern[%d].channel is :%x", i, adc_pattern.channel);
          ESP_LOGI(TAG, "adc_pattern[%d].unit is :%x", i, adc_pattern.unit);
  //    }
      dig_cfg.adc_pattern = &adc_pattern;
      ESP_ERROR_CHECK(adc_digi_controller_configure(&dig_cfg));
  }


  void Setup(void (*pOnBufferFullEvent)(void* data, int length))
  {
    esp_err_t ret;
    uint32_t ret_num = 0;
      
    setCpuFrequencyMhz(240); //set cpu to 240mhz incase it isnt already set
    semaOnBufferFull = xSemaphoreCreateBinary();
    onBufferFullEvent = pOnBufferFullEvent;
    PrimaryBuffer =  (uint32_t*)malloc(Shared::PACKET_SIZE*4); //create buffers of 32 bytes - this is the maximum size the radio can send in 1 packet
    SecondaryBuffer = (uint32_t*)malloc(Shared::PACKET_SIZE*4);
   
    xTaskCreatePinnedToCore(onBufferFull, "Buffer Full Task", 8192, NULL, 1, &bufferFullTaskHandler,0);


    continuous_adc_init(adc1_chan_mask);
  
    adc_digi_start();
    while(1) 
    {
      ret = adc_digi_read_bytes((uint8_t*)SecondaryBuffer, TIMES, &ret_num, ADC_MAX_DELAY);
      if (ret == ESP_OK) 
      {
                  
          uint32_t* temp = PrimaryBuffer;
          PrimaryBuffer = SecondaryBuffer;            
          xSemaphoreGive(semaOnBufferFull);
          SecondaryBuffer =temp;          
          
      } 
      else if (ret == ESP_ERR_TIMEOUT) 
      {
          /**
          * ``ESP_ERR_TIMEOUT``: If ADC conversion is not finished until Timeout, you'll get this return error.
          * Here we set Timeout ``portMAX_DELAY``, so you'll never reach this branch.
          */
          ESP_LOGW(TAG, "No data, increase timeout or reduce conv_num_each_intr");
          vTaskDelay(1000);
      }

    }

    adc_digi_stop();
    ret = adc_digi_deinitialize();
    assert(ret == ESP_OK);
  }

   void Teardown()
   {

   }
}