#if defined(CORE_CM7) || defined(CORE_CM4) 
#include "rpc_mpi.h"
#include "RPC.h"

/// @brief facilitates communication between M4 and M7 cores. Should call RPC.begin on both cores before using this api
namespace RPC_MPI
{
  /// @brief Sends binary data through RPC serial M4/7
  /// @param code the code so the receiving side knows what to do. If this is 's' it will be treated as a string write and pass as plain text
  /// @param data pointer to the data
  /// @param length length of the data
  void Send(const char code, const char* data,const unsigned int length)
  {
    if(code==SERIAL_OUT)
    {      
      Print(data,length);
      return;
    }
    if(length>0 && code != 0 && data!=0)
    {
      RPC.write(code);      
      RPC.write((char*)&length,4);
      RPC.write(data,length);       
    }
  }
  
  /// @brief prints the data to the RPC serial stream. 's' code is prepended to the stream. Appends '\0' to the end if it is not present
  /// @param data pointer to the data
  /// @param length length of the data
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
  
  /// @brief prints a string to the RPC serial stream. 's' code is prepended to the stream
  /// @param s string reference
  void Print(const String &s)
  {
    Print(s.c_str(),s.length()+1);
  }

  /// @brief prints an integer to the RPC serial stream. 's' code is prepended to the stream
  /// @param i the value to print
  void Print(const int i)
  {
    char str[12];
    sprintf(str, "%d", i);
    Print(str,strlen(str)+1);
  }

  /// @brief sends a message through the RPC serial stream
  /// @param m the message to send
  void SendMessage(Message m)
  {
    Send(m.code,m.data,m.length);
  }

  void(*functionPtrs[256])(Message&);

  /// @brief registers a message handler. Note; 's' is reserved for text based streams and should generally be registered to forward to Serial.print*
  /// @param code the code to register the message handler against
  /// @param functionPtr pointer to the handler function
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
    m.code = RPC.read();
    
    if (m.code >= 0) 
    {
      if(m.code==SERIAL_OUT)
      {        
        m.length = RPC.readBytesUntil('\0',buffer,sizeof(buffer));         
        m.data = buffer;        
        m.data[m.length] = '\0';
      }
      else
      { 
        int count=0;
        int length;
        
        RPC.readBytes((char*)&m.length,4);   
        RPC.readBytes(buffer,m.length);
        m.data = buffer;                  
      }
      
      HandleMessage(m);
    }
  }
  /// @brief reads and processes by handlers all of the messages in the RPC stream
  void ProcessMessages()
  {
    while (RPC.available()) {      
      ProcessMessage();
    }
  }
  
}
#endif