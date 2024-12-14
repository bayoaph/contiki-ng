#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "dev/leds.h"
#include <stdio.h>

#define UDP_PORT 1234

static struct simple_udp_connection udp_conn;
static int data_counter = 0;

PROCESS(sink_process, "Sink Mote Process");
AUTOSTART_PROCESSES(&sink_process);

static void udp_callback(struct simple_udp_connection *c,
                         const uip_ipaddr_t *sender_addr,
                         uint16_t sender_port,
                         const uip_ipaddr_t *receiver_addr,
                         uint16_t receiver_port,
                         const uint8_t *data,
                         uint16_t datalen) {
  printf("Sink: Received '%.*s' from sender\n", datalen, (char *)data);

   data_counter++;
    switch (data_counter % 3) {
        case 0:
            leds_on(LEDS_RED);
            leds_off(LEDS_YELLOW | LEDS_GREEN);
            break;
        case 1:
            leds_on(LEDS_YELLOW);
            leds_off(LEDS_RED | LEDS_GREEN);
            break;
        case 2:
            leds_on(LEDS_GREEN);
            leds_off(LEDS_RED | LEDS_YELLOW);
            break;
    }
}

PROCESS_THREAD(sink_process, ev, data) {
  PROCESS_BEGIN();

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_callback);
  printf("Sink process started.\n");

  PROCESS_END();
}