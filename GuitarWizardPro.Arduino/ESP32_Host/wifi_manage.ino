#include "wifi_manage.h" 
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

namespace Wifi
{

  //WiFiServer  server(80);  
  bool Host(char* ssid, char* password)
  { 
    
    if (!WiFi.softAP(ssid, password)) 
    {
      Serial.println("Soft AP creation failed.");
      while(1);
    }
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
     //server.begin();
    delay(2000);
    Serial.println("Server started");
    return true;
  }

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