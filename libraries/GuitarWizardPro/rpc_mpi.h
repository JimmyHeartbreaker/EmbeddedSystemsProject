#pragma once
#include "Arduino.h"

namespace RPC_MPI
{
  
  const char SERIAL_OUT='s';
  struct Message
  {
    char* data;
    unsigned int length;
    char code;
  };
 
 
  void ProcessMessages();
  void SendMessage(const Message m);
  void Send(const char code, const char* data, const unsigned int length);
  void Print(const char* data, const unsigned int length);
  void Print(const String &data);
  void Print(const int i);

  
 void RegisterMessageHandler(char code, void(*functionPtr)(Message&));
  
}