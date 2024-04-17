//#include "dac_audio.h"
#include "shared.h"
#include "rf24_transceive.h"
#include "wifi_manage.h"
#include "rpc_mpi.h"
#include "shared.h"
#include <RPC.h>
using namespace RPC_MPI;

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
  
      Radio::Transceive::SetupForReceive(&SPI1);
     // Audio::D2AC::Setup(Radio::Transceive::ReceiveData,Shared::PACKED_SAMPLES_PER_PACKET);     
      
      state = State::ACTIVE;      
      Serial.println("AUDIO_SETUP::END");
      break;
    case State::ACTIVE:
      
//      Serial.println("...");
 //     delay(1000);
      if(!sharedAppData->dataAvailable)
      {

        if(HAL_HSEM_FastTake(HSEM_ID_0) == HAL_OK)
        { 
               sharedAppData->dataAvailable = true;
     
     //     Serial.println("getting data");    
          Radio::Transceive::ReceiveData(sharedAppData->AudioBuffer,Shared::PACKET_SIZE);
   //       Serial.println("got data");
          HAL_HSEM_Release(HSEM_ID_0,0);
        }
      }

      //Audio::D2AC::WriteAudioToDMABuffer();
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
  Serial.println("<M4>");
  Serial.println(m.data);  
  Serial.println("</M4>");
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
    // HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
         
    Serial.println("RPC started");
  }
  else
  {          
    Serial.println("RPC failed");
  }

  Serial.println("Started"); 
}