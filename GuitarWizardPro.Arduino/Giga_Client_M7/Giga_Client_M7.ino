#include "dac_audio.h"
#include "shared.h"
#include "rf24_transceive.h"


enum State {
  OFF,
  SETUP,
  ACTIVE
};

enum State state = State::OFF;

void loop() 
{ 
  switch(state)
  {
    case State::OFF:
      {
        state= State::SETUP;       
    
        if(state!= State::OFF)
        {
          Serial.println("started");
        }
        break;
      }
    case State::SETUP:
      Serial.println("AUDIO_SETUP::BEGIN");
      Radio::Transceive::SetupForReceive(&SPI1);
      Audio::D2AC::Setup(Radio::Transceive::ReceiveData,Shared::PACKED_SAMPLES_PER_PACKET);     
      state = State::ACTIVE;      
      Serial.println("AUDIO_SETUP::END");
      break;
    case State::ACTIVE:
      Audio::D2AC::WriteAudioToDMABuffer();
    break;
  }
}

void setup() 
{ 
  Serial.begin(115200);   
  delay(5000);
    
  Serial.println("Started"); 
}