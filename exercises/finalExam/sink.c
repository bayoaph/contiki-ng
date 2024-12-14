#include "contiki.h"
#include "simple-udp.h"
#include "dev/leds.h"
//#include "net/uip.h"  // Include this for UIP IP stack
#include <stdio.h>
#include <string.h>

PROCESS(sink_process, "Sink Process");
AUTOSTART_PROCESSES(&sink_process);

static void udp_receive_callback(struct simple_udp_connection *c,
                                  const uip_ipaddr_t *sender_addr,
                                  uint16_t sender_port,
                                  const uip_ipaddr_t *receiver_addr,
                                  uint16_t receiver_port,
                                  const uint8_t *data,
                                  uint16_t datalen) {
  printf("Received data: %.*s from ", datalen, (char *)data);

  // Print the IP address of the sender
  printf("%02x:%02x:%02x:%02x\n",
         sender_addr->u8[0], sender_addr->u8[1], sender_addr->u8[2], sender_addr->u8[3]);

  static int state = 0;
  switch (state) {
    case 0: leds_on(LEDS_RED); leds_off(LEDS_YELLOW | LEDS_GREEN); break;
    case 1: leds_on(LEDS_YELLOW); leds_off(LEDS_RED | LEDS_GREEN); break;
    case 2: leds_on(LEDS_GREEN); leds_off(LEDS_RED | LEDS_YELLOW); break;
  }
  state = (state + 1) % 3;
}

static struct simple_udp_connection udp_conn;

PROCESS_THREAD(sink_process, ev, data) {
  PROCESS_BEGIN();

  simple_udp_register(&udp_conn, 1234, NULL, 1234, udp_receive_callback);

  while (1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
