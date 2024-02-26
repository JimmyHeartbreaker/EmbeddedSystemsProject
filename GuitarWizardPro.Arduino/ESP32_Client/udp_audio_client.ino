#include "udp_audio_client.h" 
#include <WiFi.h>

#include "AsyncUDP.h"

namespace Audio::UDP
{

  AsyncUDP udp;
  IPAddress addr(224,1,2,3);
  bool StartListeningServer()
  {
     if(udp.listenMulticast(IPAddress(224,1,2,3), 1234)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        
      return true;
      
    }
    return false;
  }

}
