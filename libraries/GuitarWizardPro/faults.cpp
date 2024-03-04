#include "faults.h"
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
namespace Faults
{
  
  Fault* printFault(Fault* fault, char* format, ...)
  {    
    if(!fault)
    {
        Serial.println("Parameter fault is null");
        return NULL;
    }
    va_list args;
    Serial.println("** Fault Log **");    
    if(format)
    {      
      char buffer[100];
      va_start(args, format);
      vsnprintf(buffer, sizeof buffer, format, args);
      va_end(args);
      Serial.print("Message:");
      Serial.println(buffer);
    }
    Serial.print("LogId:");
    Serial.println(fault->LogId);
    Serial.print("Reason:");
    Serial.println(fault->Reason);
    Serial.print("ActionToTake:");
    switch(fault->ActionToTake)
    {
      case 1: Serial.println("Retry");break;
      case 2: Serial.println("Terminate");break;
      default: Serial.println("None");break;
    }
    Serial.println("** End Log **");
    return fault;
  }
}