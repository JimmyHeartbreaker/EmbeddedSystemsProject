#pragma once


#include <ArduinoBLE.h>
#include <Arduino.h>


char* Username=0;
char* Password=0;
char* SSID=0;
char* SecurityType=0;
char* EncryptionType=0;
char* SecurityKey=0;


bool HasWifiInfo=false;

void BWA_fetchWifiInfo();
void BWA_setup();
