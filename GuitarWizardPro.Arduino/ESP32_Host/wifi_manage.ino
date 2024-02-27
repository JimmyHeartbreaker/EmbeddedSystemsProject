#include "wifi_manage.h" 
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "faults.h"

using namespace Faults;

namespace Wifi
{
 
#pragma region Faults  
  int logId = rand();

  Fault failedHost =
  {
    .ActionToTake = TERMINATE,
    .Reason = "Could not start soft access point",
    .LogId = logId
  };
  
  Fault parameterFault =
  {
    .ActionToTake = TERMINATE,
    .Reason = "parameter is null",
    .LogId = logId
  };

  Fault failedConnect =
  {
    .ActionToTake = TERMINATE,
    .Reason = "Could not connect to wifi access point",
    .LogId = logId
  };

#pragma endregion

  Fault* Host(char* ssid, char* password)
  {    
    if(!ssid)
      return printFault(&parameterFault,"SSID is null");      

    if (!WiFi.softAP(ssid, password)) 
    {
      return printFault(&failedHost,"Soft AP creation failed. SSID:'%s', passkey:'%s'",ssid,password);         
    }
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    return NULL;
  }

  Fault* Connect(char* ssid, char* passkey)
  {        
    if(!ssid)
      return printFault(&parameterFault,"SSID is null");

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