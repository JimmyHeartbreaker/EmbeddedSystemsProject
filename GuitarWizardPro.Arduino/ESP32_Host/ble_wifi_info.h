#pragma once



namespace BT::LE::WifiInfo
{
  struct WifiInfo
  { 
    char* Username=0;
    char* Password=0;
    char* SSID=0;
    char* SecurityType=0;
    char* EncryptionType=0;
    char* SecurityKey=0;
    char* ProviderIpAddress=0;

  };

  void EnableBluetooth();
  void DisableBluetooth();
  WifiInfo GetWifiInfo();
  int GetClients(String* addr);
  void Setup();
}