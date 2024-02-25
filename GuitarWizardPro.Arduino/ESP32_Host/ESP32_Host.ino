
#include "ble_wifi_info.h"
#include "wifi_manage.h"
#include "adc_audio.h"
#include "udp_audio_server.h"
using namespace BT::LE::WifiInfo;
WifiInfo wifiInfo;
String ipAddresses[10];
int ipAddressLength = 0;
bool wifiConnected=false;

enum State {
  NETWORK_DETAILS_FETCH,
  WIFI_CONNECT,
  WIFI_HOST,
  UDP_SETUP,
  ACTIVE
} ;

bool started = false;
enum State state = State::WIFI_HOST;
//int state;
void loop() 
{ 
  if(!started)
  {
    int byte = Serial.read();
    if(byte == 'A')
    {
      Serial.println("starting");
      started=true;
      
      Audio::ADC::Setup(onBufferFull);
      Serial.println("started");
    }
    return;
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
      if(Wifi::Connect(wifiInfo.SSID,wifiInfo.SecurityKey))
      {
        state = State::UDP_SETUP;
      }
      Serial.println("WIFI_CONNECT::END");
      break;
    case State::WIFI_HOST:
      Serial.println("WIFI_HOST::BEGIN");
      if(Wifi::Host("GuitarWizardPro2",NULL))
      {
        state = State::UDP_SETUP;
      }      
      Serial.println("WIFI_HOST::END");
      break;
    case State::UDP_SETUP:
      Serial.println("UDP_SETUP::BEGIN");
      if(Audio::UDP::StartServer())
      {
        state = State::ACTIVE;
      }
      Serial.println("UDP_SETUP::END");
      break;
    case State::ACTIVE:
      break;
  }
}
int frames=0;
void onBufferFull()
{
  if(state == State::ACTIVE)
  {
    
    Audio::UDP::SendAudio((uint8_t*)Audio::ADC::PrimaryBuffer,Audio::ADC::BUFFER_SIZE*2);
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