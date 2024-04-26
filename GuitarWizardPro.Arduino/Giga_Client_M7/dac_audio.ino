#include "dac_audio.h"
#include "AudioDAC.h"
#include "Shared.h"
#include "arm_math.h"
#include <stdio.h>

namespace Audio::D2AC 
{
  #define BLOCK_SIZE          16
//  #define NUM_TAPS            14
  

#define NUM_STAGE_IIR 5
#define NUM_ORDER_IIR (NUM_STAGE_IIR * 2)
#define NUM_STD_COEFFS 2 // b0, b1, b2, a1, a2

static float32_t scaler[NUM_STAGE_IIR]=
{                                                   
0.009322837517141319835234369861609593499 ,                                                       
0.008825677058542816949548992511154210661  ,                                                      
0.008442692929079948296888780134850094328   ,                                                     
0.008184648885073539728574054663567949319    ,                                                    
0.008055213335710947192280784179274633061 
};


      
static float32_t iirState[NUM_ORDER_IIR];
static float32_t iirCoeffs[NUM_STAGE_IIR * (NUM_STD_COEFFS*2+1)] = 
{
    1*scaler[0], 2*scaler[0], 1*scaler[0],  1.903478583146325231467699268250726163387, -0.940769933214890552442000171140534803271,
    1*scaler[1], 2*scaler[1], 1*scaler[1],  1.801971474008201790084626736643258482218, -0.837274182242373044005034898873418569565,
    1*scaler[2], 2*scaler[2], 1*scaler[2],  1.723776172762509384384088662045542150736,  -0.757546944478828976343720569275319576263,
    1*scaler[3], 2*scaler[3], 1*scaler[3],  1.671090355770465496476617772714234888554,  -0.703828951310759620696444471832364797592,
    1*scaler[4], 2*scaler[4], 1*scaler[4],  1.644663015847778408939916516828816384077,  -0.676883869190622045053373767586890608072
 //  -3.487307741549900, 4.589291232078406, -2.698884391340753, 0.598065261600887,
};
 


  AudioDAC dac0(A12);
  void (*copyDataToBufferFPtr)(void*, int);


  static arm_biquad_cascade_df2T_instance_f32 S1k;

  void UnpackAudio( int16_t* buffer) 
  {    
    uint16_t* outBuf = dac0.getWriteBuffer();
    float32_t signal[16];
    arm_q15_to_float (buffer,signal,16);    
    
    arm_biquad_cascade_df2T_f32(&S1k, signal,signal , BLOCK_SIZE);


    for(int i=0;i<16;i++)
    {
      //multiply by 8192 (32768/4) to get the values into the correct range
      outBuf[i] =  roundf(min(4095,signal[i]*8192));      
    }    
  //   Serial.write((char*)buffer,32);
   //  Serial.print("\r\n");
  }

  int16_t buffer[Shared::PACKET_SIZE];
  void Setup(void (*copyDataToBuffer)(void*,int), int samplesPerChannel) 
  {      
   // float32_t computedCoefs[NUM_TAPS_1k * 2 - 1];
  //  arm_biquad_cascade_df2T_compute_coefs_f32(1,coefs,computedCoefs);
  
    arm_biquad_cascade_df2T_init_f32(&S1k, NUM_STAGE_IIR, &iirCoeffs[0], &iirState[0]);
  //  arm_biquad_cascade_df2T_init_f32(&S1k, numStages, coeffs1k, firStateF321k);
   // arm_fir_init_f32(&S1k, NUM_TAPS_1k, coeffs1k, firStateF321k, BLOCK_SIZE); 
    copyDataToBufferFPtr = copyDataToBuffer;

    //init the DAC0 on the DMA controller
    if (!dac0.init(AN_RESOLUTION_12, samplesPerChannel)) 
    {
      Serial.println("Failed to start DAC0 !");   
    }
    
    //get some data just before we kick it off
    copyDataToBufferFPtr(buffer,Shared::PACKET_SIZE);
    //unpack it
    UnpackAudio(buffer);
    //start
    dac0.start();
    //we already know that the 2nd 1/2 of the circular buffer must be filled up before the 1st 1/2 has completed so lets get on with it
    copyDataToBufferFPtr(buffer,Shared::PACKET_SIZE);
    UnpackAudio(buffer);
  }

  void WriteAudioToDMABuffer() 
  {
    //wait for next buffer to come through
   
    
    copyDataToBufferFPtr(buffer,Shared::PACKET_SIZE);  
    //as soon as DMA interrupt for the next block of data occurs we can then copy that to the buffer with a bit of processing in the middle
    while (!dac0.writeRequired()) {}
    UnpackAudio(buffer);
    dac0.writeCompleted();
   
  }
}