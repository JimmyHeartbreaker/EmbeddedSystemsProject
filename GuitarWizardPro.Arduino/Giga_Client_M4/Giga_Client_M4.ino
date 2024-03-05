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

void loop() 
{ 

  switch(state)
  {
    case State::ACTIVE:        
      while(HAL_HSEM_FastTake(HSEM_ID_0) != HAL_OK)
      { 
      }
      if(sharedAppData.dataAvailable)
      {
        RPC_MPI::Print("AUDIO LENGTH:");
        RPC_MPI::Print(sharedAppData.PrimaryAudioBufferLength);
        sharedAppData.dataAvailable=false;
      }    
      delay(1);
      HAL_HSEM_Release(HSEM_ID_0,0);   
      break;
  }
}

void setup() 
{

  sharedAppData.PrimaryAudioBuffer = (int16_t*)malloc(64);
  if(RPC.begin())
  {
    delay(2000);
    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_NVIC_SetPriority(HSEM1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(HSEM1_IRQn);

    int ptr = (int)&sharedAppData;
    
    RPC_MPI::Print(sizeof(char*));
    RPC_MPI::Print((int)ptr);
    RPC_MPI::Send(Shared::APPDATA_RECIEVE, (char*)&ptr,sizeof(char*));
    RPC_MPI::Print("RPC started");
    state = State::ACTIVE;
  }
  else
  {          
    RPC_MPI::Print("RPC failed");
  }
}