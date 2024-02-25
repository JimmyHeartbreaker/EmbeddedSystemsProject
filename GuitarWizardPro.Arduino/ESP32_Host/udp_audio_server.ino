#include "udp_audio_server.h" 
#include <WiFi.h>

#include "AsyncUDP.h"

namespace Audio::UDP
{

  AsyncUDP udp;
  IPAddress addr(224,1,2,3);
  bool StartServer()
  {
     if(udp.listenMulticast(IPAddress(224,1,2,3), 1234)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
      
      return true;
      
    }
    return false;
  }
  void SendAudio(uint8_t* data, int length)
  {
    udp.writeTo(data, length,addr, 1234);
  }
}


// #include "udp_audio_server.h" 
// #include <WiFi.h>
// #include <WiFiUdp.h>

// namespace Audio::UDP
// {
//   WiFiUDP Udp;
//  // AsyncUDP udp;
//   IPAddress addr(224,1,2,3);
//   bool StartServer()
//   {
//     Udp.beginMulticast(addr,1234);
//     // if(udp.listenMulticast(IPAddress(224,1,2,3), 1234)) {
//     //    Serial.print("UDP Listening on IP: ");
//     //    Serial.println(WiFi.localIP());
      
//     //  return true;
      
    
//     return false;
//   }
//   void SendAudio(uint8_t* data, int length)
//   {
//     Udp.beginPacket(addr, 1234);

//     Udp.write(data,length);

//     Udp.endPacket();
//    // udp.writeTo(data, length,addr, 1234);
//   }
// }