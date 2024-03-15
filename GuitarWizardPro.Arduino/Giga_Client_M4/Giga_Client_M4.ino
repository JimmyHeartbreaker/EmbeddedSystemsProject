#include <RPC.h>
#include "rpc_mpi.h"
#include "stm32h7xx_hal.h"
#include "shared.h"
#include "dac_audio.h"
using namespace RPC_MPI;

enum State {
  OFF,
  ACTIVE
};

enum State state = State::OFF;
Shared::AppData sharedAppData;
int16_t* prevBuf;
bool primary=true;
bool started=false;
void loop() 
{ 
 if(primary)
  {
    if(sharedAppData.primaryLength<=0)
    {
      return;
    }
    Audio::D2AC::Write(sharedAppData.PrimaryAudioBuffer);

    primary = false;
  }
  else
  {
    if(sharedAppData.secondaryLength<=0)
    {
      return;
    }
    Audio::D2AC::Write(sharedAppData.SecondaryAudioBuffer);
  
    primary=true;
  } // while(HAL_HSEM_FastTake(HSEM_ID_0) != HAL_OK)
      // { 
      // }
      // bool shouldWrite = prevBuf!=sharedAppData.PrimaryAudioBuffer;
      
      // prevBuf = sharedAppData.PrimaryAudioBuffer;
      
      // HAL_HSEM_Release(HSEM_ID_0,0);  
      // if(shouldWrite)
      // { 
      //   
      // }
          
      
      // delay(1);
     // if(sharedAppData.dataAvailable)
     // {
      //  sharedAppData.dataAvailable = false;
      //}
     // sharedAppData.dataAvailable=false;
            
   //   delay(5);
    
  
}

const int PacketSize=480;
void setup() 
{

  sharedAppData.PrimaryAudioBuffer = (int16_t*)malloc(1440);
  
  sharedAppData.SecondaryAudioBuffer = (int16_t*)malloc(1440);
  
  if(RPC.begin())
  {
    delay(2000);
    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_NVIC_SetPriority(HSEM1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(HSEM1_IRQn);

    int ptr = (int)&sharedAppData;
    
    RPC_MPI::Send(Shared::APPDATA_RECIEVE, (char*)&ptr,sizeof(char*));
    RPC_MPI::Print("RPC started");
    Audio::D2AC::Setup();
    state = State::ACTIVE;
  }
  else
  {          
    RPC_MPI::Print("RPC failed");
  }
}