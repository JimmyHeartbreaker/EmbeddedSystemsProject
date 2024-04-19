#include "udp_audio_client.h"
#include "shared.h"
#include "rf24_transceive.h"
#include "wifi_manage.h"
#include "rpc_mpi.h"
#include "shared.h"
#include <RPC.h>
#include "faults.h"

using namespace RPC_MPI;
using namespace Faults;
enum State {
  OFF,
  SETUP,
  ACTIVE
};


Shared::AppData* sharedAppData;
enum State state = State::OFF;

void loop() 
{   
  switch(state)
  {
    case State::OFF:
      {
        RPC_MPI::ProcessMessages();
  
        break;
      }
    case State::SETUP:

      Serial.println("AUDIO_SETUP::BEGIN");
      RPC_MPI::ProcessMessages();
      // if(Fault* f = Wifi::Connect("VM5678927","jjbhpqDHs5xz"))
      // {
      //   printFault(f,"Wifi did not connect");
      //   return;
      // }
    //  Audio::UDP::Setup();
      Radio::Transceive::SetupForReceive(&SPI1);
      state = State::ACTIVE;      
      Serial.println("AUDIO_SETUP::END");
      break;
    case State::ACTIVE:
      
      if(!sharedAppData->dataAvailable)
      {
        if(HAL_HSEM_FastTake(HSEM_ID_0) == HAL_OK)
        { 
          sharedAppData->dataAvailable = true;
          Radio::Transceive::ReceiveData(sharedAppData->AudioBuffer,Shared::PACKET_SIZE);
          HAL_HSEM_Release(HSEM_ID_0,0);
       //   Audio::UDP::Send(sharedAppData->AudioBuffer,32);
        }
      }

    break;
  }
}

void receiveAppDataPtr(Message& m)
{ 
  Serial.println("received ptr to shared appData");
   
  sharedAppData = ((Shared::AppData**)m.data)[0];
  Serial.println((uint)sharedAppData);
  state= State::SETUP;       
    
}

void printMessageFromM4(Message& m)
{  
  Serial.print("<Sent from M4>\n\t");
  Serial.println(m.data);  
}


void setup() 
{ 
   Serial.begin(115200);    
  delay(5000);
  SCB_DisableDCache();
  RPC_MPI::RegisterMessageHandler(Shared::APPDATA_RECIEVE,receiveAppDataPtr);
  RPC_MPI::RegisterMessageHandler(RPC_MPI::SERIAL_OUT,printMessageFromM4);
   
  if(RPC.begin())
  {
    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_NVIC_SetPriority(HSEM1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(HSEM1_IRQn);
         
    Serial.println("RPC started");
  }
  else
  {          
    Serial.println("RPC failed");
  }

  Serial.println("Started"); 
}