#include "ble_wifi_info.h"

const char* AppName = "Guitar Wizard Pro";
const char* WifiAuthServiceUUID = "0d7322c5-377a-4f0c-0000-b1706ce81a2e";
const char* UsernameUUID = "0d7322c5-377a-4f0c-0001-b1706ce81a2e";
const char* PasswordUUID = "0d7322c5-377a-4f0c-0002-b1706ce81a2e";
const char* SSIDUUID = "0d7322c5-377a-4f0c-0003-b1706ce81a2e";
const char* SecurityTypeUUID = "0d7322c5-377a-4f0c-0004-b1706ce81a2e";
const char* EncryptionTypeUUID = "0d7322c5-377a-4f0c-0005-b1706ce81a2e";
const char* SecurityKeyUUID = "0d7322c5-377a-4f0c-0006-b1706ce81a2e";

BLEDevice FindDeviceWithService(const char* uuid)
{
  BLEDevice peripheral;
  
  while(true)
  {
    BLE.scan();    
    delay(1000);
    peripheral = BLE.available();    
  
    if (peripheral.hasAdvertisedServiceUuid()) 
    {
      for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) 
      {
        String serviceUUID = peripheral.advertisedServiceUuid(i);

        char buf[serviceUUID.length()+1];
        serviceUUID.toCharArray(buf, serviceUUID.length()+1);
       
        if(!strcmp(buf,uuid))
        {
          Serial.println("* Peripheral device found!");
          Serial.print("* Device MAC address: ");
          Serial.println(peripheral.address());
          Serial.print("* Device name: ");
          Serial.println(peripheral.localName());
          BLE.stopScan();
          return peripheral;
        }
      }
    }    
  }
  BLE.stopScan();
  return peripheral;  
}

bool ConnectAndDiscover(BLEDevice peripheral)
{
  while (!peripheral.connect())
  {
    delay(1000);
  }
  Serial.println("Discovering attributes ...");
   
  if (peripheral.discoverAttributes()) 
  {
    Serial.println("Attributes discovered");
    return true;
  } 
  else 
  {
    Serial.println("Attribute discovery failed!");
    return false;
  }
}
BLEDevice wifiProvidingDevice;

void ReadStringCharacteristic(BLEDevice device, const char* uuid, char** addr)
{
   BLECharacteristic characteristic = device.characteristic(uuid);
      
    if(characteristic.read())
    {       
      int len =characteristic.valueLength();
      char* derefAddr = *addr;
      derefAddr = (char*)malloc(len+1);
      for(int i=0;i<len+1;i++){derefAddr[i]='\0';}

      characteristic.readValue(derefAddr,len);   
      Serial.println(derefAddr);  
    }
}
void BWA_fetchWifiInfo() 
{ 
  if(!HasWifiInfo)
  {
    if(!wifiProvidingDevice.connected())
    {
      wifiProvidingDevice = FindDeviceWithService(WifiAuthServiceUUID);
      if(!ConnectAndDiscover(wifiProvidingDevice))
      {        
        wifiProvidingDevice.disconnect();  
        return;
      }
    }
    
    ReadStringCharacteristic(wifiProvidingDevice,UsernameUUID,&Username);    
    ReadStringCharacteristic(wifiProvidingDevice,PasswordUUID,&Password);    
    ReadStringCharacteristic(wifiProvidingDevice,SSIDUUID,&SSID);    
    ReadStringCharacteristic(wifiProvidingDevice,SecurityTypeUUID,&SecurityType);    
    ReadStringCharacteristic(wifiProvidingDevice,EncryptionTypeUUID,&EncryptionType);    
    ReadStringCharacteristic(wifiProvidingDevice,SecurityKeyUUID,&SecurityKey);

    HasWifiInfo = true;
    wifiProvidingDevice.disconnect();  
  }

}

void BWA_setup() 
{
  Serial.println("BLE Setup started");
  if (!BLE.begin()) 
  {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    return;
  }

  BLE.setLocalName(AppName);
  BLE.setDeviceName(AppName);


  Serial.println("BLE Setup Completed");
}