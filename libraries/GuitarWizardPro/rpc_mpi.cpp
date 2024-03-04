#include "rpc_mpi.h"
#include "RPC.h"
namespace RPC_MPI
{
  Message ReadMessage()
  {
    Message m;
    char buffer[128];
    while (RPC.available()) {
      m.length = RPC.readBytesUntil('\0',buffer,sizeof(buffer));  

      if ( m.length > 0) {
        m.code = buffer[0];
        
        if(m.code==SERIAL_OUT)
        {
          m.data = (char*)malloc(m.length);
          memcpy(m.data,buffer+1,m.length-1);
          m.data[m.length-1] = '\0';
        }
        else
        { 
          m.data = (char*)malloc(m.length-1);
          memcpy(m.data,buffer+1,m.length-1);          
        }
      }
      return m;
    }
    return m;

  }

  

  void Send(const char code, const char* data,const unsigned int length)
  {
    if(length>0 && code != 0 && data!=0)
    {
      RPC.write(code);
      RPC.write(data,length);
      RPC.write('\0');     
    }
  }

  void Print(const char* data,const unsigned int length)
  { 
    if(length>0 && data != 0)
    {
      RPC.write(SERIAL_OUT);
      RPC.write(data,length);
      if(data[length-1] != '\0')
      {
        RPC.write('\0');
      }
    }
  }

  void Print(const String &data)
  {
    Print(data.c_str(),data.length()+1);
  }

  
  void Print(const int i)
  {
    char str[10];
    sprintf(str, "%d", i);
    Print(str,sizeof(str));
  }

  void SendMessage(Message m)
  {
    Send(m.code,m.data,m.length);
  }

   void(*functionPtrs[256])(Message&);

  void RegisterMessageHandler(char code, void(*functionPtr)(Message&))
  {
    functionPtrs[code] = functionPtr;
  }

  bool HandleMessage(Message& m)
  {
    if(m.length>0)
    {
      void(*functionPtr)(Message&) = functionPtrs[m.code];
      if(functionPtr)
      {
        functionPtr(m);        
        return true;
      }
    }
    
    return false;
  }

  void ProcessMessage()
  {
    Message m;
    char buffer[128];
    m.length = RPC.readBytesUntil('\0',buffer,sizeof(buffer));  

    if ( m.length > 0) {
      m.code = buffer[0];
      
      if(m.code==SERIAL_OUT)
      {
        m.data =buffer+1;
        m.data[m.length-1] = '\0';
      }
      else
      { 
        m.data = buffer+1;          
      }
      HandleMessage(m);
    }
  }
  void ProcessMessages()
  {
    while (RPC.available()) {      
      ProcessMessage();
    }
  }
  
}