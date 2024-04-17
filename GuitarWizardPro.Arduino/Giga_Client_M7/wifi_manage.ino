#include "wifi_manage.h" 
#include <WiFi.h>
#include "faults.h"

using namespace Faults;

namespace Wifi
{
 
#pragma region Faults  
  int logId = rand();

  Fault failedHost =
  {
    ActionToTake : TERMINATE,
    Reason : "Could not start soft access point",
    LogId : logId
  };
  
  Fault parameterFault =
  {
    ActionToTake : TERMINATE,    
    Reason : "parameter is null",
    LogId : logId
  };

  Fault failedConnect =
  {
    ActionToTake : TERMINATE,
    Reason : "Could not connect to wifi access point",
    LogId : logId
  };
Fault notImplemented =
  {
    ActionToTake : TERMINATE,
    Reason : "Wifi hosting is not implemented here",
    LogId : logId
  };
#pragma endregion

  Fault* Host(char* ssid, char* password)
  {    
    return &notImplemented;
  }

  Fault* Connect(char* ssid, char* passkey)
  {        
    if(!ssid)
      return printFault(&parameterFault,"SSID is null");

    if (WiFi.status() == WL_NO_MODULE) {
      Serial.println("Communication with WiFi module failed!");
      // don't continue
      while (true);
    }
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, passkey);
    int status;
    do 
    {
      delay(500);
      status = WiFi.status();
      switch(status)
      {
        case WL_CONNECT_FAILED:
        case WL_NO_SSID_AVAIL:
        case WL_CONNECTION_LOST:
        case WL_NO_SHIELD:
        case WL_SCAN_COMPLETED:
          return printFault(&failedConnect,"Could not connect to WiFi. SSID:'%s', passkey:'%s', status:%i",ssid,passkey,status);          
        case WL_IDLE_STATUS:
        case WL_DISCONNECTED: 
          break;
      } 
    }while(status != WL_CONNECTED);

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    return NULL;
  }  

  void Disconnect()
  {
    Serial.println("WiFi disconnecting...");
    WiFi.disconnect();
    Serial.println("WiFI disconnected");
  }
}