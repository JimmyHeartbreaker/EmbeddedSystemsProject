// #include <dummy.h>

// #include <driver/adc.h>
// #include "adc_audio_dma.h"
// #include <math.h>
// #include <freertos/FreeRTOS.h>
// #include <driver/i2s.h>
// #include <SPIFFS.h>
// #include <FS.h>
//  #include <string.h>
// #include <stdio.h>
// #include "sdkconfig.h"
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/semphr.h"
// #include "driver/adc.h"
// #include "udp_audio_server.h"
// #define GET_UNIT(x)        ((x>>3) & 0x1)
// #define ADC_RESULT_BYTE     2
// #define ADC_CONV_LIMIT_EN   1
// #define ADC_CONV_MODE       ADC_CONV_SINGLE_UNIT_1 
// #define ADC_OUTPUT_TYPE     ADC_DIGI_OUTPUT_FORMAT_TYPE1


// namespace Audio::ADC::DMA
// {

//   pbuf* SecondaryBuffer;
//   static uint16_t adc1_chan_mask = BIT(0);
//   static uint16_t adc2_chan_mask = 0;
//   static adc1_channel_t channel[1] = {ADC1_CHANNEL_0};
  
//   SemaphoreHandle_t  semaOnBufferFull; 

//   static const char *TAG = "ADC DMA";

//   static void continuous_adc_init(uint16_t adc1_chan_mask, uint16_t adc2_chan_mask, adc1_channel_t *channel, uint8_t channel_num)
//   {
//       adc_digi_init_config_t adc_dma_config = {
//           .max_store_buf_size = 4000,
//           .conv_num_each_intr = BUFFER_SIZE*2,
//           .adc1_chan_mask = adc1_chan_mask,
//           .adc2_chan_mask = adc2_chan_mask,
//       };
//       ESP_ERROR_CHECK(adc_digi_initialize(&adc_dma_config));

//       adc_digi_configuration_t dig_cfg = {
//           .conv_limit_en = ADC_CONV_LIMIT_EN,
//           .conv_limit_num = 250,
//           .sample_freq_hz = 32000,
//           .conv_mode = ADC_CONV_MODE,
//           .format = ADC_OUTPUT_TYPE,
//       };

//       adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
//       dig_cfg.pattern_num = channel_num;
//       for (int i = 0; i < channel_num; i++) {
//           uint8_t unit = GET_UNIT(channel[i]);
//           uint8_t ch = channel[i] & 0x7;
//           adc_pattern[i].atten = ADC_ATTEN_DB_2_5;
//           adc_pattern[i].channel = ch;
//           adc_pattern[i].unit = unit;
//           adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;

//           ESP_LOGI(TAG, "adc_pattern[%d].atten is :%x", i, adc_pattern[i].atten);
//           ESP_LOGI(TAG, "adc_pattern[%d].channel is :%x", i, adc_pattern[i].channel);
//           ESP_LOGI(TAG, "adc_pattern[%d].unit is :%x", i, adc_pattern[i].unit);
//       }
//       dig_cfg.adc_pattern = adc_pattern;
//       ESP_ERROR_CHECK(adc_digi_controller_configure(&dig_cfg));
//   }

//   #if !CONFIG_IDF_TARGET_ESP32
//   static bool check_valid_data(const adc_digi_output_data_t *data)
//   {
//       const unsigned int unit = data->type2.unit;
//       if (unit > 2) return false;
//       if (data->type2.channel >= SOC_ADC_CHANNEL_NUM(unit)) return false;

//       return true;
//   }
//   #endif
//   void swapBuffers()
//   {
//       pbuf* temp = PrimaryBuffer;
//       PrimaryBuffer = SecondaryBuffer;
//       SecondaryBuffer = temp;     
//   }
//   void continuousReadADC(void *params)
//   {
//     Serial.println(xPortGetCoreID());
//       esp_err_t ret;
//       uint32_t ret_num = 0;
//       memset((uint8_t*)SecondaryBuffer->payload, 0xcc, BUFFER_SIZE*2);

//       continuous_adc_init(adc1_chan_mask, adc2_chan_mask, channel, sizeof(channel) / sizeof(adc1_channel_t));
//       adc_digi_start();

//       while(1) {
//         //  Serial.println("reading");
//         ret = adc_digi_read_bytes((uint8_t*)SecondaryBuffer->payload, BUFFER_SIZE*2, &ret_num, ADC_MAX_DELAY);
//         if(ret_num>0)
//         {
//           swapBuffers();
//           xSemaphoreGive(semaOnBufferFull);     
//         }
//           if (ret == ESP_OK ) {
//           //  Serial.println("sending");
              
//           //     Serial.println("sent");
//              // 
//           //    Serial.println(ret_num);
//           //    Serial.println(ret );

//              // for (int i = 0; i < ret_num/2; i ++) {
//               //    adc_digi_output_data_t *p = (adc_digi_output_data_t*)&((uint16_t*)SecondaryBuffer->payload)[i];
//      // #if CONFIG_IDF_TARGET_ESP32
//               //      Serial.println( p->type1.data);
//       // #else
//       //             if (ADC_CONV_MODE == ADC_CONV_BOTH_UNIT || ADC_CONV_MODE == ADC_CONV_ALTER_UNIT) {
//       //                 if (check_valid_data(p)) {
//       //                     ESP_LOGI(TAG, "Unit: %d,_Channel: %d, Value: %x", p->type2.unit+1, p->type2.channel, p->type2.data);
//       //                 } else {
//       //                     // abort();
//       //                     ESP_LOGI(TAG, "Invalid data [%d_%d_%x]", p->type2.unit+1, p->type2.channel, p->type2.data);
//       //                 }
//       //             }
//       // #if CONFIG_IDF_TARGET_ESP32S2
//       //             else if (ADC_CONV_MODE == ADC_CONV_SINGLE_UNIT_2) {
//       //                 ESP_LOGI(TAG, "Unit: %d, Channel: %d, Value: %x", 2, p->type1.channel, p->type1.data);
//       //             } else if (ADC_CONV_MODE == ADC_CONV_SINGLE_UNIT_1) {
//       //                 ESP_LOGI(TAG, "Unit: %d, Channel: %d, Value: %x", 1, p->type1.channel, p->type1.data);
//       //             }
//       // #endif  //#if CONFIG_IDF_TARGET_ESP32S2
//       // #endif
              
//               //See `note 1`
//               //vTaskDelay(1);
//           } else if (ret == ESP_ERR_TIMEOUT) {
//           Serial.println("timeout");
//               /**
//               * ``ESP_ERR_TIMEOUT``: If ADC conversion is not finished until Timeout, you'll get this return error.
//               * Here we set Timeout ``portMAX_DELAY``, so you'll never reach this branch.
//               */
//               ESP_LOGW(TAG, "No data, increase timeout or reduce conv_num_each_intr");
//               vTaskDelay(1000);
//           }
//           else if (ret == ESP_ERR_INVALID_STATE) {
//           Serial.println("invalid");
//                   /**
//                   * @note 1
//                   * Issue:
//                   * As an example, we simply print the result out, which is super slow. Therefore the conversion is too
//                   * fast for the task to handle. In this condition, some conversion results lost.
//                   *
//                   * Reason:
//                   * When this error occurs, you will usually see the task watchdog timeout issue also.
//                   * Because the conversion is too fast, whereas the task calling `adc_digi_read_bytes` is slow.
//                   * So `adc_digi_read_bytes` will hardly block. Therefore Idle Task hardly has chance to run. In this
//                   * example, we add a `vTaskDelay(1)` below, to prevent the task watchdog timeout.
//                   *
//                   * Solution:
//                   * Either decrease the conversion speed, or increase the frequency you call `adc_digi_read_bytes`
//                   */
//               }

//       }

//       adc_digi_stop();
//       ret = adc_digi_deinitialize();
//       assert(ret == ESP_OK);
//   }

//   void (*onBufferFullEvent)();

// void continuousSendUDP(void *params)
//   {

//     Serial.println(xPortGetCoreID());
//     while(true)
//     {
//       xSemaphoreTake( semaOnBufferFull,pdMS_TO_TICKS(100000) );  
//       Audio::UDP::Send(PrimaryBuffer);  
//      // Serial.println("sent");
//       vTaskDelay(pdMS_TO_TICKS(5));
//     }
//   }
//   TaskHandle_t continuousReadADCTaskHandler;
//   TaskHandle_t continuousSendUDPTaskHandler;
//   void Setup(void (*pOnBufferFullEvent)())
//   {  
//     adc1_config_width(ADC_WIDTH_BIT_12);
//     adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_2_5);    
//     analogRead(ADC1_CHANNEL_0);

//     semaOnBufferFull = xSemaphoreCreateBinary();
//     onBufferFullEvent = pOnBufferFullEvent;
//     PrimaryBuffer = pbuf_alloc(PBUF_TRANSPORT, BUFFER_SIZE*2, PBUF_RAM);
//     SecondaryBuffer = pbuf_alloc(PBUF_TRANSPORT, BUFFER_SIZE*2, PBUF_RAM);
//     xTaskCreatePinnedToCore(continuousReadADC, "continuous Read ADC", 8192, NULL, 2, &continuousReadADCTaskHandler,0);
//     xTaskCreatePinnedToCore(continuousSendUDP, "continuous send udp", 8192, NULL, 1, &continuousSendUDPTaskHandler,1);
   


//   }

//   void Teardown()
//   {
   
//     onBufferFullEvent = NULL;
//   }
// }