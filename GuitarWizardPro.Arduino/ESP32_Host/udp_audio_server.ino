/*
this is the implementation for audio over udp.
the goals are to be real time and no breaks in the audio

*/

#include "udp_audio_server.h" 
#include <WiFi.h>
#include "faults.h"
#include "AsyncUDP_2.h"

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

  AsyncUDP2 udp;
  IPAddress addr(224,1,2,3);

  
  void Setup()
  {
    udp.connect(addr,1234);
  }
  Fault* Send(pbuf* data)
  {
    static float successCount=0;
    static int errorCount=0;
    if(!udp.writeBuf(data))
    {
      errorCount++;
      
      double errorRate = errorCount/(successCount+errorCount)*100.0;
      
      if(errorRate<10)
      {
        return printFault(&failedSend,"Send failed with data length %i",data->len);
      }
      return printFault(&critErrorSend,"Send failed with data length %i. Error rate over 10%",data->len);
    }
    
    //successCount++;
    
    return NULL;
  }
}


