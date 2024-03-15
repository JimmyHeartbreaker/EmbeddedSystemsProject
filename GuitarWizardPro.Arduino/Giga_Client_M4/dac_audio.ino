#include "dac_audio.h" 
#include <DigitalOut.h>
#include "rpc_mpi.h"
#include <Arduino_AdvancedAnalog.h>

namespace Audio::D2AC
{
  AdvancedDAC dac_out(A12);
  void Setup()
  { 
     if (!dac_out.begin(AN_RESOLUTION_12, 32000 , 720, 32)) {
        RPC_MPI::Print("Failed to start DAC0 !");
        while (1);
    }
  }
  void Write(int16_t* data)
  { 
  

    //  for(int j=0;j<15;j++,data+=48)
     // {
       // while(!dac_out.available())
       // {

       // }
        // Get a free buffer for writing.
          SampleBuffer buf = dac_out.dequeue();

          // Write data to buffer.
          memcpy(buf.data(),data,1440);
         //  for (int i=0; i<buf.size(); i++) {
           //    buf.data()[i] = data[i];
          // }

          // Write the buffer to DAC.
          dac_out.write(buf);
        
   //   }
     
  }
  
}
