#include "contiki.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h"
#include <stdio.h>
#include <string.h>
#include "sys/log.h"

#define LOG_MODULE "Sink"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_SERVER_PORT 1234
#define UDP_CLIENT_PORT 4321

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(sink_process, "Sink Process");
AUTOSTART_PROCESSES(&sink_process);
/*---------------------------------------------------------------------------*/
static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen) {
  char received_data[64];
  memset(received_data, 0, sizeof(received_data));

  // Copy received data into a string for printing
  snprintf(received_data, sizeof(received_data), "%.*s", datalen, (char *)data);

  printf("Temperature received: ");
  uip_debug_ipaddr_print(sender_addr);
  printf(": %s\n", received_data);

  static int state = 0;

switch (state) {
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

// Cycle to the next state
state = (state + 1) % 3;

}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sink_process, ev, data) {
  PROCESS_BEGIN();
  
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL, UDP_CLIENT_PORT, udp_rx_callback);
  LOG_INFO("Processing. Wait for data...\n");

  while (1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
