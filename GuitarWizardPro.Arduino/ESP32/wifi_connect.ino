#include "wifi_connect.h" 
BluetoothA2DPSource a2dp_source;

//callback used by A2DP to provide the sound data
int32_t get_sound_data(Channels* data, int32_t len) {
   return 0;
}


void WC_connect(char* ssid)
{
  
}

void WC_setup()
{
  a2dp_source.start("MyMusic", get_sound_data);
  
}