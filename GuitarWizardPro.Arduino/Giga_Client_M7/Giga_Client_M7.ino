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
 //  int i=0;
 //  while(i<Shared::SAMPLES_PER_PACKET)
  // {
  //    if (radio.isChipConnected())
    //  {
     //   Serial.println("connected");
      while(!radio.available()){}
       // if( radio.available()  ) 
        {
          radio.read(buffer, 32);
          
        //      i+=16;
          //if(temp[0]==4096)
          //{
        
        //  }
        
    //    radio.flush_rx();
        }
     // }
//   }
 //   Serial.println(buffer[0]);
          
  //Serial.println("packet received");
  //;
  // int length=0;
  // do
  // {
  //   length  = Audio::UDP::Receive(buffer,Shared::SAMPLES_PER_PACKET*2);
  // }while(length<=0);
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
 //     if(!HandleFault(Wifi::Connect("GuitarWizardPro",NULL)))
   //   {
        state = State::AUDIO_SETUP;            
    //  }

      Serial.println("WIFI_CONNECT::END");
      break;
    case State::AUDIO_SETUP:
      Serial.println("AUDIO_SETUP::BEGIN");
    //  Audio::UDP::Setup();     
   
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
  
//  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(D10,OUTPUT); 
 // pinMode(D7,OUTPUT); 
  
  SPI1.begin();
  while(!radio.begin(&SPI1))
  {
    Serial.println("failed to init radio");
    delay(100);
  } 
  
  radio.setChannel(124);
 // radio.setPayloadSize(3);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_2MBPS );
    
radio.openWritingPipe(0xF0F0F0F0F1);
  radio.openReadingPipe(1,0xF0F0F0F0F0);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  radio.printPrettyDetails();
while(!radio.isChipConnected())
  {

  //  Serial.println("chip is not connected");
    delay(100);
  }
   Serial.println("Started"); 
}