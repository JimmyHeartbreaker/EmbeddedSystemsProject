#include "wifi_manage.h" 
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

namespace Wifi
{

  

  bool Connect(char* ssid, char* passkey)
  {
    
    Serial.println();
    Serial.println("******************************************************");
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, passkey);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

   

    return true;
  }
}