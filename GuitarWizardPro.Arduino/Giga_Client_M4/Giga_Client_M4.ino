#include <RPC.h>
#include "rpc_mpi.h"
#include "stm32h7xx_hal.h"
#include "shared.h"
using namespace RPC_MPI;

enum State {
  OFF,
  ACTIVE
};

enum State state = State::OFF;
Shared::AppData sharedAppData;

// void HAL_HSEM_FreeCallback(uint32_t SemMask)
// {	
//   RPC_MPI::Print("callback invoked");  
// }

void loop() 
{ 

  switch(state)
  {
    case State::ACTIVE:        
      while(HAL_HSEM_FastTake(HSEM_ID_0) != HAL_OK)
      { 
      }

      RPC_MPI::Print("semaphore acquired M4");     
      delay(1000);
      HAL_HSEM_Release(HSEM_ID_0,0);    
      delay(1000);
      
      
      //out put sharedAppDatta.PrimaryAudioBuffer
      break;
  }
}

void setup() 
{

  sharedAppData.PrimaryAudioBuffer = (int16_t*)malloc(1024);
  if(RPC.begin())
  {
    delay(2000);
    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_NVIC_SetPriority(HSEM1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(HSEM1_IRQn);

  //  HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
    RPC_MPI::Send(Shared::APPDATA_RECIEVE, (char*)&sharedAppData,sizeof(char*));
    RPC_MPI::Print("RPC started");
    state = State::ACTIVE;
  }
  else
  {          
    RPC_MPI::Print("RPC failed");
  }
}