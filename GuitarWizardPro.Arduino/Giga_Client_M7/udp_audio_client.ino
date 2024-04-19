#include "udp_audio_client.h"
#include "SocketHelpers.h"
#include "api/Udp.h"

#include "netsocket/SocketAddress.h"
#include "netsocket/UDPSocket.h"
#include "WiFi.h"

namespace Audio::UDP
{
  
  UDPSocket _socket;          // Mbed OS socket
  
  SocketAddress _remoteHost;  // Remote host that sent incoming packets
  IPAddress ipAddr(224,1,2,3);
  int port = 1234;

  SocketAddress socketAddressFromIpAddress(IPAddress ip, uint16_t port) {
    nsapi_addr_t convertedIP = { NSAPI_IPv4, { ip[0], ip[1], ip[2], ip[3] } };
    return SocketAddress(convertedIP, port);
  }

  uint8_t beginOnPort(uint16_t port) 
  {
  // success = 1, fail = 0

    nsapi_error_t rt = _socket.open(WiFi.getNetwork());
    if (rt != NSAPI_ERROR_OK) {
      return 0;
    }

    if (_socket.bind(port) < 0) {
      return 0;  //Failed to bind UDP Socket to port
    }


    _socket.set_blocking(false);
    _socket.set_timeout(0);

    return 1;
  }

  void Setup()
  {    
    if (beginOnPort(port) != 1) 
    {  
      Serial.println("Begin port failed\n");
    
      return;
    }
    SocketAddress socketAddress = socketAddressFromIpAddress(ipAddr, port);
  
    if (_socket.connect(socketAddress) != NSAPI_ERROR_OK) 
    {
     Serial.println("Error joining the multicast group\n");
     return;
    }
  
  }

  void Send(void* buffer, int length)
  {
    _socket.send(buffer,length);
  }
  int Receive(void* buffer,int length)
  {
    return _socket.recv(buffer, length);
  }
}