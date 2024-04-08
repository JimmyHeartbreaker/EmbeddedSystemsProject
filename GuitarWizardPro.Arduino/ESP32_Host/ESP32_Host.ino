#include <dummy.h>
#include "driver/adc.h"
 #include "esp32-hal-adc.h"
 #include <esp_adc_cal.h>


#include "adc_audio.h"
#include "faults.h"
#include <stdarg.h>
#include <stdio.h>
#include "rf24_transceive.h"
#include "adc_audio_cont.h"
using namespace Faults;


enum State {
  OFF,
  SETUP,
  ACTIVE,
  RESET
} ;


enum State initialState = State::SETUP;
enum State state = State::SETUP;
bool waitingForResponse = false;


bool HandleFault(Fault* f)
{
  if(f)
  {
    if(f->ActionToTake == TERMINATE)
    {
      state = State::OFF;
    }
    return true;
  }
  return false;
}

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
      Radio::Transceive::Setup();
      Audio::ADC::Setup(Radio::Transceive::SendData);   
    //  Audio::ADC::DMA::Setup(Radio::Transceive::SendData);   
     
      state = State::ACTIVE;      
      Serial.println("SETUP::END");
      break;      
    case State::RESET:
      Serial.println("RESET::BEGIN");
 
      Audio::ADC::Teardown();
      state = initialState;
      
      Serial.println("RESET::END");
      break;
    case State::ACTIVE:
      
      break;
  }
}   

void setup() 
{
  Serial.begin(115200);
  delay(5000);
  Serial.println("started");
 
}