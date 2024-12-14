#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "dev/leds.h"
#include <stdio.h>
#include "net/ipv6/uip-debug.h"

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1"

static struct simple_udp_connection udp_conn;


struct temperatureMessage
{
  char messageString[50];
  uint16_t temperature;
};

/*---------------------------------------------------------------------------*/
PROCESS(sink_process, "Sink mote process");
AUTOSTART_PROCESSES(&sink_process);
/*---------------------------------------------------------------------------*/
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen) {
  struct temperatureMessage received_data;
  
  memcpy(&received_data, data, sizeof(struct temperatureMessage)); // Deserialize

  int16_t temp = received_data.temperature;
  printf("%s  ", received_data.messageString);
  uip_debug_ipaddr_print(sender_addr);
  printf("\nReceived temperature: %d.%d°C\n", temp / 10, temp % 10);

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


PROCESS_THREAD(sink_process, ev, data)
{
  PROCESS_BEGIN();

  /* Register UDP connection */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

  while (1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
