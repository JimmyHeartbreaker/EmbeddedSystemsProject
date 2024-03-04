#include "udp_audio_client.h" 
#include <WiFi.h>
#include <SPI.h>
#include <WiFiUdp.h>



namespace Audio::UDP
{
  
  WiFiUDP udp;
  IPAddress addr(224,1,2,3);


  void Setup()
  {    
    udp.beginMulticast(addr,1234);
  }
  int Receive(char* buffer)
  {
    int packetSize = udp.parsePacket();
    if (packetSize) {
      return udp.read(buffer, 512); 
    }
    return NULL;
  }
}
