#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "net/ipv6/uip-debug.h"
#include "net/ipv6/uip-ds6.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1"

/* A message format to hold temperature data and an associated string */
struct temp_msg {
  char     msg_str[50];
  uint16_t temperature;
};

// Forward declaration
static void update_leds(void);

PROCESS(sink_process, "Data sink process");
AUTOSTART_PROCESSES(&sink_process);

static struct simple_udp_connection udp_conn;
static int led_state = 0;

static void update_leds() {
  leds_off(LEDS_ALL);
  switch(led_state) {
    case 0:
      leds_on(LEDS_RED);
      break;
    case 1:
      leds_on(LEDS_RED);
      leds_on(LEDS_GREEN); // Red + Green = Yellow
      break;
    case 2:
      leds_on(LEDS_GREEN);
      break;
  }
}

static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen)
{
  struct temp_msg rx_data;

  // Copy the incoming packet data into our struct
  memcpy(&rx_data, data, sizeof(rx_data));

  // Decode the temperature (stored in tenths of degrees)
  int16_t received_temp = rx_data.temperature;
  int16_t integer_part = received_temp / 10;
  int16_t fractional_part = received_temp % 10;

  // Print the message string and the sender's IP address
  printf("%s  ", rx_data.msg_str);
  uip_debug_ipaddr_print(sender_addr);
  printf("\nReceived temperature: %d.%d°C\n", integer_part, fractional_part);

  // Cycle through LED states
  led_state = (led_state + 1) % 3;
  update_leds();
}

PROCESS_THREAD(sink_process, ev, data)
{
  static uip_ipaddr_t ipaddr;
  PROCESS_BEGIN();

  // Assign a manual global address (if needed)
  uip_ip6addr(&ipaddr,0xfd00,0,0,0,0,0,0,1);
  uip_ds6_addr_add(&ipaddr,0,ADDR_MANUAL);

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

  update_leds();

  // Keep the process running to continuously handle events
  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
