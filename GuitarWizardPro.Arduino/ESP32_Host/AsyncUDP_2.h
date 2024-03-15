#ifndef ESPAsyncUDP2_H
#define ESPAsyncUDP2_H

#include "IPAddress.h"
#include "Print.h"
#include "Stream.h"
#include <functional>
extern "C" {
#include "esp_netif.h"
#include "lwip/ip_addr.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
}



class AsyncUDP2;
struct udp_pcb;
struct pbuf;



class AsyncUDP2
{
protected:
    udp_pcb *_pcb;
    bool _connected;
	  esp_err_t _lastErr;
    void _recv(udp_pcb *upcb, pbuf *pb, const ip_addr_t *addr, uint16_t port);
    bool _init();

public:
    AsyncUDP2();
    virtual ~AsyncUDP2();

    bool connect(const ip_addr_t *addr, uint16_t port);
    bool connect(const IPAddress addr, uint16_t port);

    void close();

    bool writeBuf(pbuf *data);

    bool connected();
	  esp_err_t lastErr();
    static void _s_recv(void *arg, udp_pcb *upcb, pbuf *p, const ip_addr_t *addr, uint16_t port);

};

#endif