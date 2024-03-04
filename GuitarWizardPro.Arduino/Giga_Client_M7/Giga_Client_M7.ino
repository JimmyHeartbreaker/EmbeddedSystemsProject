#include "wifi_manage.h"
#include "udp_audio_client.h"
#include <RPC.h>
#include "rpc_mpi.h"
#include "shared.h"
using namespace RPC_MPI;

enum State {
  OFF,
  WIFI_CONNECT,
  UDP_SETUP,
  ACTIVE
};

Shared::AppData* sharedAppData;
int16_t* secondaryAudioBuffer;
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

void loop() 
{ 
  RPC_MPI::ProcessMessages();

  delay(500);
  if(state == State::OFF)
  { 
    int mode = Serial.read();
    
    switch(mode)
    {
      case 'c':        
        state= State::WIFI_CONNECT;
        break;
        
      case 't':        
        while(HAL_HSEM_FastTake(HSEM_ID_0)!=HAL_OK)
        {
        }
        
        Serial.println("Semaphore acquired M7");
        break;
        
      case 'r':        
        HAL_HSEM_Release(HSEM_ID_0,0);    
        break;
      default:
        break;
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
      //if(Fault* fault = Wifi::Connect("VM5678927_EXT","jjbhpqDHs5xz"))
      if(!HandleFault(Wifi::Connect("GuitarWizardPro",NULL)))
      {
        state = State::UDP_SETUP;            
      } 

      Serial.println("WIFI_CONNECT::END");
      break;
    case State::UDP_SETUP:
      Serial.println("UDP_SETUP::BEGIN");
      Audio::UDP::Setup();      
      state = State::ACTIVE;      
      Serial.println("UDP_SETUP::END");
      break;
    case State::ACTIVE:
      int length = Audio::UDP::Receive((char*)secondaryAudioBuffer);
      if(HAL_HSEM_FastTake(HSEM_ID_0))
      {
        int16_t* temp =  sharedAppData->PrimaryAudioBuffer;
        sharedAppData->PrimaryAudioBuffer = secondaryAudioBuffer;
        sharedAppData->PrimaryAudioBufferLength = length;
        secondaryAudioBuffer = temp;
        HAL_HSEM_Release(HSEM_ID_0,0);
      }
    break;
  }
}
void receiveAppDataPtr(Message& m)
{ 
  Serial.println("received ptr to shared appData");
   
  sharedAppData = (Shared::AppData*)m.data;
}

void printMessageFromM4(Message& m)
{  
  Serial.println("<M4>");
  Serial.println(m.data);  
  Serial.println("</M4>");
}
void setup() 
{ 
  Serial.begin(115200);    
  delay(5000);
  secondaryAudioBuffer = (int16_t*)malloc(1024);  
  RPC_MPI::RegisterMessageHandler(Shared::APPDATA_RECIEVE,receiveAppDataPtr);
  RPC_MPI::RegisterMessageHandler(RPC_MPI::SERIAL_OUT,printMessageFromM4);
   
  if(RPC.begin())
  {

    __HAL_RCC_HSEM_CLK_ENABLE();

    HAL_NVIC_SetPriority(HSEM1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(HSEM1_IRQn);
    // HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
         
    Serial.println("RPC started");
  }
  else
  {          
    Serial.println("RPC failed");
  }
}