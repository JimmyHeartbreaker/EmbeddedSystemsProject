#include <dummy.h>

#include <driver/adc.h>
#include "adc_audio.h"
#include <math.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

namespace Radio::Transceive
{

  RF24 radio(9, 10,20000000); 



  void IRAM_ATTR SendData(void* data, int length)
  {
      radio.writeFast(data, length);         
  }
  
  void Setup()
  {  
    pinMode(D10,OUTPUT);
    while(!radio.begin())
    {
        Serial.println("radio did not connect. Retrying in 1 second...");
        delay(1000);
    }
    
    radio.setPALevel(RF24_PA_MIN);
    radio.setRetries(0,0);
    radio.setDataRate(RF24_2MBPS );
    radio.setAutoAck(false);
   radio.setChannel(124);
   radio.openWritingPipe(0xF0F0F0F0F0);   
  radio.openReadingPipe(1,0xF0F0F0F0F1);

  radio.stopListening();
  
  radio.printPrettyDetails();
    while(!radio.isChipConnected())
    {
        Serial.println("chip not connected1");
        delay(100);
    }

  }

  void Teardown()
  {
    }
}