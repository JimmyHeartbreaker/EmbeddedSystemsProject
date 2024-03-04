#pragma once
#include <stdarg.h>
#include <stdio.h>
namespace Faults
{
  enum FaultAction
  {
    NONE,
    RETRY,
    TERMINATE
  };

  struct Fault
  {
    FaultAction ActionToTake;
    char* Reason;
    int LogId;
  };

  Fault* printFault(Fault* fault,  char* formatMessage, ...);
}

