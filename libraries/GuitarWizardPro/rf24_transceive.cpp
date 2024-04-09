#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

namespace Radio::Transceive
{

  RF24 radio(9, 10,20000000); 
  
  const uint64_t pipes[2] = {   0xF0F0F0F0F1LL, 0xF0F0F0F0F2LL };



  void SendData(void* data, int length)
  {
      radio.writeFast(data, length);         
  }
  
  
  void ReceiveData(void* buffer, int length)
  {
    while(!radio.isChipConnected()){}
    while(!radio.available()){}
    radio.read(buffer, length);      
  }

  void setup(SPIClass* spi) 
  { 
    pinMode(D10,OUTPUT); 
   
    spi->begin();
    while(!radio.begin(spi))
    {
      Serial.println("failed to init radio, retrying in 1 second...");
      delay(1000);
    }
    radio.setRetries(0,0);
    radio.setChannel(124);
    radio.setAutoAck(false);
    radio.setDataRate(RF24_2MBPS ); 
    radio.setPALevel(RF24_PA_MIN);
    
  }
  void SetupForReceive(SPIClass* spi)
  {
    setup(spi);
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
    radio.startListening();
    
    radio.printPrettyDetails();
    
    while(!radio.isChipConnected())
    {
      Serial.println("chip is not connected");
      delay(1000);
    }    
  }
  
  void SetupForTransmit(SPIClass* spi)
  {  
    setup(spi);

   radio.openWritingPipe(pipes[0]);   
  radio.openReadingPipe(1,pipes[1]);

  radio.stopListening();
  
  radio.printPrettyDetails();
    while(!radio.isChipConnected())
    {
        Serial.println("chip not connected1");
        delay(1000);
    }

  }

  void Teardown()
  {
  }
}