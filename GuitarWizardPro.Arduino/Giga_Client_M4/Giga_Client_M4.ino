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
  
    //   
  
      if(sharedAppData.dataAvailable){
      
        if(HAL_HSEM_FastTake(HSEM_ID_0) == HAL_OK)
        { 
      
    //   RPC_MPI::Print("data available");  
        //delay(10000);
            Audio::D2AC::UnpackAndStream(sharedAppData.AudioBuffer);
            sharedAppData.dataAvailable = false;
          
            HAL_HSEM_Release(HSEM_ID_0,0);
        
      //  RPC_MPI::Print("released");  
        } 
      }
  
}

void setup() 
{  
  if(RPC.begin())
  {
    delay(2000);
    __HAL_RCC_HSEM_CLK_ENABLE();
    HAL_NVIC_SetPriority(HSEM1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(HSEM1_IRQn);
    sharedAppData.x = 12345;
    
    sharedAppData.AudioBuffer = (uint8_t*)malloc(32);
    
    uint ptr = (uint)&sharedAppData;
    
    RPC_MPI::Print(sizeof(char*));
    RPC_MPI::Print(ptr);
    RPC_MPI::Send(Shared::APPDATA_RECIEVE, (char*)&ptr,sizeof(char*));
    RPC_MPI::Print("RPC started");
    
    Audio::D2AC::Setup(Shared::PACKED_SAMPLES_PER_PACKET);
    state = State::ACTIVE;
  }
  else
  {          
    RPC_MPI::Print("RPC failed");
  }
}