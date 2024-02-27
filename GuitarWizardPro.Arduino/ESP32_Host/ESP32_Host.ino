
#include "ble_wifi_info.h"
#include "wifi_manage.h"
#include "adc_audio.h"
#include "udp_audio_server.h"
#include "faults.h"
#include <stdarg.h>
#include <stdio.h>

using namespace BT::LE::WifiInfo;
using namespace Faults;

WifiInfo wifiInfo;
String ipAddresses[10];
int ipAddressLength = 0;

enum State {
  OFF,
  NETWORK_DETAILS_FETCH,
  WIFI_CONNECT,
  WIFI_HOST,
  ADC_SETUP,
  ACTIVE,
  RESET
} ;

enum State state = State::OFF;
bool waitingForResponse = false;
void loop() 
{ 
  if(state == State::OFF)
  { 
    if(!waitingForResponse)
    {
     Serial.println("Please select 1 of the following options:");
     Serial.println("h - Host a wifi access point with the SSID 'GuitarWizardPro'");
     Serial.println("c - Connect to the hardcoded access point");
     Serial.println("f - Fetch wifi details from a BLE service and connect to the provided access point");
     waitingForResponse = true;
    }
    int mode = Serial.read();
    waitingForResponse = false;
    switch(mode)
    {
      case 'h':
        state = State::WIFI_HOST;
        break;
      case 'c':
        state= State::WIFI_CONNECT;
        break;
      case 'f':
        state = State::NETWORK_DETAILS_FETCH;
        break;
      default:
        waitingForResponse = true;
        break;
    }
    if(state!= State::OFF)
    {
      Serial.println("started");
    }
    else
      return;
  }
  else if (Serial.available() > 0) {
    int mode = Serial.read();
    switch(mode)
    {
      case 'r':
        state = State::RESET;
        break;
      default:break;
    }
  }

  switch(state)
  {
    case State::NETWORK_DETAILS_FETCH:
      Serial.println("NETWORK_DETAILS_FETCH::BEGIN");
      wifiInfo = GetWifiInfo();
      
      ipAddressLength = GetClients(ipAddresses+1) + 1;
      Serial.print("Sound device count:");
      Serial.println(ipAddressLength);

      if(!wifiInfo.SSID)
      {
        Serial.println("Failed to acquire wifi. Starting AP");
        state = State::WIFI_HOST;
      }
      else
      {        
        Serial.println("Wifi info acquired.");
        Serial.print("* SSID: ");
        Serial.println(wifiInfo.SSID);
        Serial.print("* ProviderIpAddress: ");
        Serial.println(wifiInfo.ProviderIpAddress);
        state = State::WIFI_CONNECT;
      }
      
      BT::LE::WifiInfo::DisableBluetooth(); 
      Serial.println("NETWORK_DETAILS_FETCH::END");
      break;
    case State::WIFI_CONNECT:      
      Serial.println("WIFI_CONNECT::BEGIN");
      ipAddresses[0] = wifiInfo.ProviderIpAddress;
      if(Fault* fault = Wifi::Connect(wifiInfo.SSID,wifiInfo.SecurityKey))
      {
        if(fault->ActionToTake == TERMINATE)
        {
          state = State::OFF;
        }
        else
        {
          return;
        }
      }
      else
      {        
        state = State::ADC_SETUP;
      }
      Serial.println("WIFI_CONNECT::END");
      break;
    case State::WIFI_HOST:
      Serial.println("WIFI_HOST::BEGIN");
      
      if(Fault* fault = Wifi::Host("GuitarWizardPro",NULL))
      {
        if(fault->ActionToTake == TERMINATE)
        {
          state = State::OFF;
        }
        else
        {
          return;
        }
      }
      else
      {        
        state = State::ADC_SETUP;
      }
      Serial.println("WIFI_HOST::END");
      break;
    case State::ADC_SETUP:
      Serial.println("ADC_SETUP::BEGIN");
      
      Audio::ADC::Setup(onBufferFull);   
      state = State::ACTIVE;      
      Serial.println("ADC_SETUP::END");
      break;      
    case State::RESET:
      Serial.println("RESET::BEGIN");
 
      Wifi::Disconnect();
      Audio::ADC::Teardown();
      state = State::OFF;
      
      Serial.println("RESET::END");
      break;
    case State::ACTIVE:
      break;
  }
}
int frames=0;
void onBufferFull()
{
  if(Fault* fault = Audio::UDP::Send((uint8_t*)Audio::ADC::PrimaryBuffer,Audio::ADC::BUFFER_SIZE*2))
  {
    if(fault->ActionToTake == TERMINATE)
    {
      state = RESET;
    }
  }  
}
void setup() 
{
  delay(2000);
  Serial.begin(115200);
  Serial.println("Setup started");

  wifiInfo.SSID = "VM5678927_EXT";
  wifiInfo.ProviderIpAddress = "192.168.0.56";
  wifiInfo.SecurityKey = "jjbhpqDHs5xz";

  Serial.println("Setup Finished");
}