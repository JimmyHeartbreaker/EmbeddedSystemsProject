#include "udp_audio_server.h" 
#include <WiFi.h>
#include "faults.h"
#include "AsyncUDP.h"
using namespace Faults;
namespace Audio::UDP
{
#pragma region Faults  
  int logId = rand();

  Fault failedStartServer =
  {
    .ActionToTake = RETRY,
    .Reason = "Could not connect to server",
    .LogId = logId
  };
  Fault failedSend =
  {
    .ActionToTake = RETRY,
    .Reason = "udp.writeTo failed",
    .LogId = logId
  };
  Fault critErrorSend =
  {
    .ActionToTake = TERMINATE,
    .Reason = "udp.writeTo error rate > 10%",
    .LogId = logId
  };
#pragma endregion

  AsyncUDP udp;
  IPAddress addr(224,1,2,3);

  Fault* Send(uint8_t* data, int length)
  {
    static float successCount=0;
    static int errorCount=0;
    if(!udp.writeTo(data, length,addr, 1234))
    {
      errorCount++;
      
      double errorRate = errorCount/(successCount+errorCount)*100.0;
      
      if(errorRate<10)
      {
        return printFault(&failedSend,"Send failed with data length %i",length);
      }
      return printFault(&critErrorSend,"Send failed with data length %i. Error rate over 10%",length);
    }
    
    successCount++;
    
    return NULL;
  }
}
