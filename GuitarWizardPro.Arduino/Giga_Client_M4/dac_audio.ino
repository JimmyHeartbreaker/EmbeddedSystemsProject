#include "dac_audio.h" 
#include <DigitalOut.h>
#include <Arduino_AdvancedAnalog.h>

namespace Audio::D2AC
{
  AdvancedDAC dac0(A12);
  void Setup()
  {
     if (!dac0.begin(AN_RESOLUTION_12, 32000, 32, 64)) {
        Serial.println("Failed to start DAC0 !");
        while (1);
    }
  }

  void Write(int16_t* data, int length)
  {
     if (dac0.available()) 
     {
      
        // Get a free buffer for writing.
        SampleBuffer buf = dac0.dequeue();
        
        // Write data to buffer.
        
        
        // Write the buffer to DAC.
      //  dac_out.write(buf);
    }
  }
  
}
