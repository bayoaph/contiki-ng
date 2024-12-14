#include "contiki.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h"
#include <stdio.h>
#include <string.h>

#define UDP_SERVER_PORT 5678
#define UDP_CLIENT_PORT 8765

static struct simple_udp_connection udp_conn;

PROCESS(sink_process, "UDP Sink Process");
AUTOSTART_PROCESSES(&sink_process);

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

  // Print the received data
  printf("Data received from ");
  uip_debug_ipaddr_print(sender_addr);
  printf(": %s\n", received_data);

  // Toggle LED color every time data is received
  static int led_state = 0;
  if (led_state == 0) {
    leds_on(LEDS_RED);
    leds_off(LEDS_GREEN);
    leds_off(LEDS_YELLOW);
    led_state = 1;
  } else if (led_state == 1) {
    leds_on(LEDS_YELLOW);
    leds_off(LEDS_RED);
    leds_off(LEDS_GREEN);
    led_state = 2;
  } else {
    leds_on(LEDS_GREEN);
    leds_off(LEDS_RED);
    leds_off(LEDS_YELLOW);
    led_state = 0;
  }
}

PROCESS_THREAD(sink_process, ev, data) {
  PROCESS_BEGIN();

  // Initialize UDP connection
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL, UDP_CLIENT_PORT, udp_rx_callback);

  printf("Sink process started. Waiting for data...\n");

  PROCESS_END();
}
