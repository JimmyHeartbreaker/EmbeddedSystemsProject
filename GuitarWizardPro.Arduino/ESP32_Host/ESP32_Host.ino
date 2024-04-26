#include "adc_audio.h"
#include "adc_dma_audio.h"
#include "rf24_transceive.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_wifi.h"


enum State {
  OFF,
  SETUP,
  ACTIVE,
  RESET
};

//this is the initial state and the state to revert to incase of a crash
enum State initialState = State::SETUP;
enum State state = State::SETUP;
bool waitingForResponse = false;

void loop() 
{ 
  if(state == State::OFF)
  { 
    if(!waitingForResponse)
    {
      Serial.println("Please select 1 of the following options:");
      Serial.println("s - setup radio and broadcast adc");
      waitingForResponse = true;
    }
    int mode = Serial.read();
    waitingForResponse = false;
    switch(mode)
    {
      case 's':
        state = State::SETUP;
        break;
      default:
        waitingForResponse = true;
        break;
    }
    if(state!= State::OFF)
    {
      Serial.println("started");
    }
    else
      return;
  }
  else if (Serial.available() > 0) {
    int mode = Serial.read();
    switch(mode)
    {
      case 'r':
        state = State::RESET;
        break;
      default:break;
    }
  }

  switch(state)
  {
    case State::SETUP:
      Serial.println("SETUP::BEGIN");
      Radio::Transceive::SetupForTransmit(&SPI);
      Audio::ADC::Setup(Radio::Transceive::SendData);   
     
      state = State::ACTIVE;      
      Serial.println("SETUP::END");
      break;      
    case State::RESET:
      Serial.println("RESET::BEGIN");
      Radio::Transceive::Teardown();
      Audio::ADC::Teardown();
      state = initialState;
      
      Serial.println("RESET::END");
      break;
    case State::ACTIVE:
      //we dont need to do anything here because the actions are running in interrupts controlled by the adc logic
      break;
  }
}   

void setup() 
{
  Serial.begin(115200);  
  delay(5000);
  esp_bluedroid_disable();
  esp_bt_controller_disable();
  esp_wifi_stop();
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  Serial.println("Arduino Nano ESP32 has booted");
 
}