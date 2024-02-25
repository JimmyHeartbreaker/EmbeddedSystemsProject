#include "ble_wifi_info.h"
#include <ArduinoBLE.h>
#include <Arduino.h>

namespace BT::LE::WifiInfo {
const char* AppName = "Guitar Wizard Pro";
const String IpServiceUUID = "0d7322c5-377a-4f0c-0010-b1706ce81a2e";
const char* IpAddressUUID = "0d7322c5-377a-4f0c-0011-b1706ce81a2e";

const String WifiAuthServiceUUID = "0d7322c5-377a-4f0c-0000-b1706ce81a2e";
const char* UsernameUUID = "0d7322c5-377a-4f0c-0001-b1706ce81a2e";
const char* PasswordUUID = "0d7322c5-377a-4f0c-0002-b1706ce81a2e";
const char* SSIDUUID = "0d7322c5-377a-4f0c-0003-b1706ce81a2e";
const char* SecurityTypeUUID = "0d7322c5-377a-4f0c-0004-b1706ce81a2e";
const char* EncryptionTypeUUID = "0d7322c5-377a-4f0c-0005-b1706ce81a2e";
const char* SecurityKeyUUID = "0d7322c5-377a-4f0c-0006-b1706ce81a2e";

bool Contains(BLEDevice* devices, int length, String deviceName) {
  for (int i = 0; i < length; i++) {
    if (!devices[i].localName().compareTo(deviceName)) {
      return true;
    }
  }
  return false;
}
void Expand(BLEDevice** devices, int newSize) {
  BLEDevice* oldArray = *devices;
  int size = sizeof(BLEDevice);
  (*devices) = (BLEDevice*)malloc(size * newSize);
  memcpy(*devices, oldArray, size * newSize / 2);
  free(oldArray);
}
int FindDevicesWithService(const String uuid, BLEDevice** foundDevices) {
  int arrayLength = 2;

  (*foundDevices) = (BLEDevice*)malloc(sizeof(BLEDevice) * arrayLength);  //start with a legnth of 2 because we only expect 2 in our demo

  int deviceCount = 0;
  int iterations = 0;
  while (iterations++ < 5) {
    BLE.scan();
    delay(500);
    BLEDevice peripheral = BLE.available();   

    if (peripheral.hasAdvertisedServiceUuid()) {
      for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
        String serviceUUID = peripheral.advertisedServiceUuid(i);
        if (!serviceUUID.compareTo(uuid)) {
          Serial.println("* Peripheral device found!");
          Serial.print("* Device MAC address: ");
          Serial.println(peripheral.address());
          Serial.print("* Device name: ");
          Serial.println(peripheral.localName());

          if (!Contains(*foundDevices, deviceCount, peripheral.localName())) {
            if (deviceCount > arrayLength - 1) {
              arrayLength *= 2;
              Serial.println("Array limit reached, expanding...");
              Expand(foundDevices, arrayLength);
            }

            (*foundDevices)[deviceCount] = peripheral;
            deviceCount++;
            iterations--;
          } else {
            Serial.println("device already found");
          }
        }
      }
    }
  }
  BLE.stopScan();
  return deviceCount;
}

bool FindDeviceWithService(const String uuid,BLEDevice* outBLEDevice ) {
  int attempts=0;
  while (attempts++<10) {
    BLE.scan();
    delay(500);
    BLEDevice peripheral = BLE.available();
   
    if (peripheral.hasAdvertisedServiceUuid()) {
      for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
        String serviceUUID = peripheral.advertisedServiceUuid(i);

        if (!serviceUUID.compareTo(uuid)) {
          Serial.println("* Peripheral device found!");
          Serial.print("* Device MAC address: ");
          Serial.println(peripheral.address());
          Serial.print("* Device name: ");
          Serial.println(peripheral.localName());
          BLE.stopScan();
          (*outBLEDevice) = peripheral;
          return true;
        }
      }
    }
  }
  BLE.stopScan();
  return false;
}

bool ConnectAndDiscover(BLEDevice peripheral) {
  while (!peripheral.connect()) {
    delay(1000);
  }
  Serial.println("Discovering attributes ...");

  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
    return true;
  } else {
    Serial.println("Attribute discovery failed!");
    return false;
  }
}

void ReadStringCharacteristic(BLEDevice device, const char* uuid, char** addr) {
  BLECharacteristic characteristic = device.characteristic(uuid);

  if (characteristic.read()) {
    int len = characteristic.valueLength();

    (*addr) = (char*)malloc(len + 1);
    for (int i = 0; i < len + 1; i++) { (*addr)[i] = '\0'; }

    characteristic.readValue((*addr), len);
  }
}
WifiInfo GetWifiInfo() {
  WifiInfo wifiInfo;
  BLEDevice wifiProvidingDevice;
  if(!FindDeviceWithService(WifiAuthServiceUUID,&wifiProvidingDevice))
  {
    return wifiInfo;
  }

  if (!ConnectAndDiscover(wifiProvidingDevice)) {
    wifiProvidingDevice.disconnect();
    Serial.println("ConnectAndDiscover failed");
    return wifiInfo;
  }
  ReadStringCharacteristic(wifiProvidingDevice, UsernameUUID, &wifiInfo.Username);
  ReadStringCharacteristic(wifiProvidingDevice, PasswordUUID, &wifiInfo.Password);
  ReadStringCharacteristic(wifiProvidingDevice, SSIDUUID, &wifiInfo.SSID);
  ReadStringCharacteristic(wifiProvidingDevice, SecurityTypeUUID, &wifiInfo.SecurityType);
  ReadStringCharacteristic(wifiProvidingDevice, EncryptionTypeUUID, &wifiInfo.EncryptionType);
  ReadStringCharacteristic(wifiProvidingDevice, SecurityKeyUUID, &wifiInfo.SecurityKey);
  ReadStringCharacteristic(wifiProvidingDevice, IpAddressUUID, &wifiInfo.ProviderIpAddress);

  wifiProvidingDevice.disconnect();
  return wifiInfo;
}

 int GetClients(String* ipAddresses)
 {
  BLEDevice* devices;
  int length = FindDevicesWithService(IpServiceUUID, &devices);
  if(length ==0)
  {
    free(devices);
    return 0;
  }

  for (int i = 0; i < length; i++) {
    BLEDevice device = devices[i];
    if (!ConnectAndDiscover(device)) {
      device.disconnect();
      continue;
    }
    char* ipAddress;
    ReadStringCharacteristic(device, IpAddressUUID, &ipAddress);
    ipAddresses[i] = String(ipAddress);
    free(ipAddress);
    device.disconnect();
  }
  free(devices);
  return length;
}

void EnableBluetooth() {
  Serial.println("BLE Setup started");
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    return;
  }

  BLE.setLocalName(AppName);
  BLE.setDeviceName(AppName);
}
void DisableBluetooth() {
  Serial.println("BLE Setup started");
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    return;
  }

  BLE.setLocalName(AppName);
  BLE.setDeviceName(AppName);
}
}