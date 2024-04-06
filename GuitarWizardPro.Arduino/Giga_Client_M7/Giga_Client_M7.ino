#include "wifi_manage.h"
#include "udp_audio_client.h"
#include "dac_audio.h"
#include "shared.h"

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

RF24 radio(9, 10,20000000); // CE, CSN
  const uint64_t pipes[2] = {   0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

enum State {
  OFF,
  SETUP,
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

void PullDataFromUDPToDAC(uint8_t* buffer)
{
  while(!radio.isChipConnected()){}
  while(!radio.available()){}
  radio.read(buffer, 32);      
}

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
   
      Audio::D2AC::Setup(PullDataFromUDPToDAC,Shared::PACKED_SAMPLES_PER_PACKET);     
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
  printf_begin();
  delay(5000);
  pinMode(D10,OUTPUT); 
  
  SPI1.begin();
  while(!radio.begin(&SPI1))
  {
    Serial.println("failed to init radio");
    delay(100);
  } 
  
  radio.setChannel(124);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_2MBPS );
    
  radio.openWritingPipe(0xF0F0F0F0F1);
  radio.openReadingPipe(1,0xF0F0F0F0F0);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  radio.printPrettyDetails();
  
  while(!radio.isChipConnected())
  {
    Serial.println("chip is not connected");
    delay(100);
  }
  
  Serial.println("Started"); 
}