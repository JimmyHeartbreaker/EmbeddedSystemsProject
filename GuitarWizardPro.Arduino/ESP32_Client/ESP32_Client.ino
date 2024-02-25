
//#include "ble_wifi_info.h"
#include "wifi_connect.h"
#include "adc_audio.h"


void loop() 
{
//  BWA_fetchWifiInfo();
//  if(HasWifiInfo)
 // {
  //  WC_Connect(SSID);
 // }
 }

void setup() 
{
  delay(2000);
  Serial.begin(115200);
  Serial.println("Setup started");
  //BWA_setup();
  AA_setup();
  WC_setup();
  Serial.println("Setup Finished");
}