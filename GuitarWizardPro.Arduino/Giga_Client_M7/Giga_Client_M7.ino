#include "wifi_manage.h"
#include "udp_audio_client.h"
#include "dac_audio.h"
#include "shared.h"

enum State {
  OFF,
  WIFI_CONNECT,
  AUDIO_SETUP,
  ACTIVE
};

enum State state = State::OFF;
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
bool high=true;
void PullDataFromUDPToDAC(uint16_t* buffer)
{
  int length=0;
  do
  {
    length  = Audio::UDP::Receive(buffer,Shared::SAMPLES_PER_PACKET*2);
  }while(length<=0);
}
void loop() 
{ 
  switch(state)
  {
    case State::OFF:
      {
      //  int mode = Serial.read();
      
      //  if(mode=='c')
      //  {    
            state= State::WIFI_CONNECT;       
      //  }
        if(state!= State::OFF)
        {
          Serial.println("started");
        }
        break;
      }
    case State::WIFI_CONNECT:      
      Serial.println("WIFI_CONNECT::BEGIN");
      //if(!HandleFault(Wifi::Connect("VM5678927","jjbhpqDHs5xz")))
      if(!HandleFault(Wifi::Connect("GuitarWizardPro",NULL)))
      {
        state = State::AUDIO_SETUP;            
      }

      Serial.println("WIFI_CONNECT::END");
      break;
    case State::AUDIO_SETUP:
      Serial.println("AUDIO_SETUP::BEGIN");
      Audio::UDP::Setup();     
   
      Audio::D2AC::Setup(PullDataFromUDPToDAC,Shared::SAMPLES_PER_PACKET);     
      state = State::ACTIVE;      
      Serial.println("AUDIO_SETUP::END");
      break;
    case State::ACTIVE:
      Audio::D2AC::SendToOutput();
    break;
  }
}

void setup() 
{ 
  Serial.begin(115200);    
  delay(5000);
   Serial.println("Started"); 
}