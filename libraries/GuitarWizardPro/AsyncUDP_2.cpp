#if defined(ESP32) 
#include "Arduino.h"
#include "AsyncUDP_2.h"

extern "C" {
#include "lwip/opt.h"
#include "lwip/inet.h"
#include "lwip/udp.h"
#include "lwip/igmp.h"
#include "lwip/ip_addr.h"
#include "lwip/mld6.h"
#include "lwip/prot/ethernet.h"
#include <esp_err.h>
#include <esp_wifi.h>
}

#include "lwip/priv/tcpip_priv.h"

typedef struct {
    struct tcpip_api_call_data call;
    udp_pcb * pcb;
    const ip_addr_t *addr;
    uint16_t port;
    struct pbuf *pb;
    struct netif *netif;
    err_t err;
} udp_api_call_t;

static err_t _udp_connect_api(struct tcpip_api_call_data *api_call_msg){
    udp_api_call_t * msg = (udp_api_call_t *)api_call_msg;
    msg->err = udp_connect(msg->pcb, msg->addr, msg->port);
    return msg->err;
}

static err_t _udp_connect(struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port){
    udp_api_call_t msg;
    msg.pcb = pcb;
    msg.addr = addr;
    msg.port = port;
    tcpip_api_call(_udp_connect_api, (struct tcpip_api_call_data*)&msg);
    return msg.err;
}

static err_t _udp_disconnect_api(struct tcpip_api_call_data *api_call_msg){
    udp_api_call_t * msg = (udp_api_call_t *)api_call_msg;
    msg->err = 0;
    udp_disconnect(msg->pcb);
    return msg->err;
}

static void  _udp_disconnect(struct udp_pcb *pcb){
    udp_api_call_t msg;
    msg.pcb = pcb;
    tcpip_api_call(_udp_disconnect_api, (struct tcpip_api_call_data*)&msg);
}

static err_t _udp_remove_api(struct tcpip_api_call_data *api_call_msg){
    udp_api_call_t * msg = (udp_api_call_t *)api_call_msg;
    msg->err = 0;
    udp_remove(msg->pcb);
    return msg->err;
}

static void  _udp_remove(struct udp_pcb *pcb){
    udp_api_call_t msg;
    msg.pcb = pcb;
    tcpip_api_call(_udp_remove_api, (struct tcpip_api_call_data*)&msg);
}

static err_t _udp_bind_api(struct tcpip_api_call_data *api_call_msg){
    udp_api_call_t * msg = (udp_api_call_t *)api_call_msg;
    msg->err = udp_bind(msg->pcb, msg->addr, msg->port);
    return msg->err;
}

static err_t _udp_bind(struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port){
    udp_api_call_t msg;
    msg.pcb = pcb;
    msg.addr = addr;
    msg.port = port;
    tcpip_api_call(_udp_bind_api, (struct tcpip_api_call_data*)&msg);
    return msg.err;
}

static err_t _udp_sendto_api(struct tcpip_api_call_data *api_call_msg){
    udp_api_call_t * msg = (udp_api_call_t *)api_call_msg;
    msg->err = udp_sendto(msg->pcb, msg->pb, msg->addr, msg->port);
    return msg->err;
}

static err_t _udp_sendto(struct udp_pcb *pcb, struct pbuf *pb, const ip_addr_t *addr, u16_t port){
    udp_api_call_t msg;
    msg.pcb = pcb;
    msg.addr = addr;
    msg.port = port;
    msg.pb = pb;
    tcpip_api_call(_udp_sendto_api, (struct tcpip_api_call_data*)&msg);
    return msg.err;
}

static err_t _udp_sendto_if_api(struct tcpip_api_call_data *api_call_msg){
    udp_api_call_t * msg = (udp_api_call_t *)api_call_msg;
    msg->err = udp_sendto_if(msg->pcb, msg->pb, msg->addr, msg->port, msg->netif);
    return msg->err;
}

static err_t _udp_sendto_if(struct udp_pcb *pcb, struct pbuf *pb, const ip_addr_t *addr, u16_t port, struct netif *netif){
    udp_api_call_t msg;
    msg.pcb = pcb;
    msg.addr = addr;
    msg.port = port;
    msg.pb = pb;
    msg.netif = netif;
    tcpip_api_call(_udp_sendto_if_api, (struct tcpip_api_call_data*)&msg);
    return msg.err;
}

typedef struct {
        void *arg;
        udp_pcb *pcb;
        pbuf *pb;
        const ip_addr_t *addr;
        uint16_t port;
        struct netif * netif;
} lwip_event_packet_t;

static QueueHandle_t _udp_queue;
static volatile TaskHandle_t _udp_task_handle = NULL;

void AsyncUDP2::_s_recv(void *arg, udp_pcb *upcb, pbuf *p, const ip_addr_t *addr, uint16_t port)
{
    reinterpret_cast<AsyncUDP2*>(arg)->_recv(upcb, p, addr, port);
}

void AsyncUDP2::_recv(udp_pcb *upcb, pbuf *pb, const ip_addr_t *addr, uint16_t port)
{
    while(pb != NULL) {
        pbuf * this_pb = pb;
        pb = pb->next;
        this_pb->next = NULL;
        
        pbuf_free(this_pb);
    }
}

static void _udp_task(void *pvParameters){
    lwip_event_packet_t * e = NULL;
    for (;;) {
        if(xQueueReceive(_udp_queue, &e, portMAX_DELAY) == pdTRUE){
            if(!e->pb){
                free((void*)(e));
                continue;
            }
            AsyncUDP2::_s_recv(e->arg, e->pcb, e->pb, e->addr, e->port);
            free((void*)(e));
        }
    }
    _udp_task_handle = NULL;
    vTaskDelete(NULL);
}

static bool _udp_task_start(){
    if(!_udp_queue){
        _udp_queue = xQueueCreate(32, sizeof(lwip_event_packet_t *));
        if(!_udp_queue){
            return false;
        }
    }
    if(!_udp_task_handle){
        xTaskCreateUniversal(_udp_task, "async_udp", 4096, NULL, CONFIG_ARDUINO_UDP_TASK_PRIORITY, (TaskHandle_t*)&_udp_task_handle, CONFIG_ARDUINO_UDP_RUNNING_CORE);
        if(!_udp_task_handle){
            return false;
        }
    }
    return true;
}

static bool _udp_task_post(void *arg, udp_pcb *pcb, pbuf *pb, const ip_addr_t *addr, uint16_t port)
{
    if(!_udp_task_handle || !_udp_queue){
        return false;
    }
    lwip_event_packet_t * e = (lwip_event_packet_t *)malloc(sizeof(lwip_event_packet_t));
    if(!e){
        return false;
    }
    e->arg = arg;
    e->pcb = pcb;
    e->pb = pb;
    e->addr = addr;
    e->port = port;
    if (xQueueSend(_udp_queue, &e, portMAX_DELAY) != pdPASS) {
        free((void*)(e));
        return false;
    }
    return true;
}

static void _udp_recv(void *arg, udp_pcb *pcb, pbuf *pb, const ip_addr_t *addr, uint16_t port)
{
    while(pb != NULL) {
        pbuf * this_pb = pb;
        pb = pb->next;
        this_pb->next = NULL;
        if(!_udp_task_post(arg, pcb, this_pb, addr, port)){
            pbuf_free(this_pb);
        }
    }
}

static bool _udp_task_stop(){
    if(!_udp_task_post(NULL, NULL, NULL, NULL, 0)){
        return false;
    }
    while(_udp_task_handle){
        vTaskDelay(10);
    }

    lwip_event_packet_t * e;
    while (xQueueReceive(_udp_queue, &e, 0) == pdTRUE) {
        if(e->pb){
            pbuf_free(e->pb);
        }
        free((void*)(e));
    }
    vQueueDelete(_udp_queue);
    _udp_queue = NULL;
}

bool AsyncUDP2::_init(){
    if(_pcb){
        return true;
    }
    _pcb = udp_new();
    if(!_pcb){
        return false;
    }
    udp_recv(_pcb, &_udp_recv, (void *) this);
    return true;
}

AsyncUDP2::AsyncUDP2()
{
    _pcb = NULL;
    _connected = false;
	_lastErr = ERR_OK;
}

AsyncUDP2::~AsyncUDP2()
{
    close();
    udp_recv(_pcb, NULL, NULL);
    _udp_remove(_pcb);
    _pcb = NULL;
}

bool AsyncUDP2::writeBuf(pbuf * pbt)
{       
    udp_send(_pcb, pbt);
    return true;
}

void AsyncUDP2::close()
{
    if(_pcb != NULL) {
        if(_connected) {
            _udp_disconnect(_pcb);
        }
        _connected = false;
        //todo: unjoin multicast group
    }
}

bool AsyncUDP2::connect(const ip_addr_t *addr, uint16_t port)
{
    if(!_udp_task_start()){
        log_e("failed to start task");
        return false;
    }
    if(!_init()) {
        return false;
    }
    close();
    _lastErr = _udp_connect(_pcb, addr, port);
    if(_lastErr != ERR_OK) {
        return false;
    }
    _connected = true;
    return true;
}


bool AsyncUDP2::connect(const IPAddress addr, uint16_t port)
{
    ip_addr_t daddr;
    daddr.type = IPADDR_TYPE_V4;
    daddr.u_addr.ip4.addr = (uint32_t)addr;
    return connect(&daddr, port);
}

bool AsyncUDP2::connected()
{
    return _connected;
}

esp_err_t AsyncUDP2::lastErr() {
	return _lastErr;
}

#endif