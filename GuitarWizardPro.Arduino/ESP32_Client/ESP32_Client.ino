
#include "wifi_manage.h"
#include "udp_audio_client.h"

enum State {
  OFF,
  WIFI_CONNECT,
  UDP_SETUP,
  ACTIVE
} ;

enum State state = State::OFF;

void loop() 
{ 
  if(state == State::OFF)
  {
    int mode = Serial.read();
    switch(mode)
    {
      case 'c':
        state= State::WIFI_CONNECT;
        break;
      default:break;
    }
    if(state!= State::OFF)
    {
      Serial.println("started");
    }
    else
      return;
  }

  switch(state)
  {
    case State::WIFI_CONNECT:      
      Serial.println("WIFI_CONNECT::BEGIN");
      if(Wifi::Connect("GuitarWizardPro",NULL))
      {
        state = State::UDP_SETUP;
      }
      Serial.println("WIFI_CONNECT::END");
      break;
    case State::UDP_SETUP:
      Serial.println("UDP_SETUP::BEGIN");
      if(Audio::UDP::StartListeningServer())
      {
        state = State::ACTIVE;
      }
      Serial.println("UDP_SETUP::END");
      break;
    case State::ACTIVE:
      break;
  }
}
void setup() 
{
  delay(2000);
  Serial.begin(115200);
  Serial.println("Setup started");

 
  Serial.println("Setup Finished");
}