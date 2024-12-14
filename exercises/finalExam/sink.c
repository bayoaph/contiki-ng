
#include "contiki.h"
#include "net/ipv6/simple-udp.h" //for udp connection
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>

#define UDP_PORT 1234

/*---------------------------------------------------------------------------*/
PROCESS(sink_mote_process, "Sink Mote Process");
AUTOSTART_PROCESSES(&sink_mote_process);
/*---------------------------------------------------------------------------*/

struct temperatureMessage {
  uint16_t source_node;
  uint16_t temperature;
};

//for LEDS changing
static uint8_t led_state = 0;

/*---------------------------------------------------------------------------*/
// Callback function to handle received UDP messages
static void udp_callback(struct simple_udp_connection *c,
                         const uip_ipaddr_t *sender_addr,
                         uint16_t sender_port,
                         const uip_ipaddr_t *receiver_addr,
                         uint16_t receiver_port,
                         const uint8_t *data,
                         uint16_t datalen) {
  struct temperatureMessage received_data;

    // deserialize the received data
    memcpy(&received_data, data, sizeof(struct temperatureMessage));
    printf("Received temperature of %u.%03u  from Node %u \n",received_data.temperature / 1000, received_data.temperature % 1000, received_data.source_node);

    // toggling LEDs from red, yellow, green alternately
    switch (led_state) {
      case 0:
        leds_on(LEDS_RED);
        leds_off(LEDS_YELLOW | LEDS_GREEN);
        led_state = 1;
        break;
      case 1:
        leds_on(LEDS_YELLOW);
        leds_off(LEDS_RED | LEDS_GREEN);
        led_state = 2;
        break;
      case 2:
        leds_on(LEDS_GREEN);
        leds_off(LEDS_RED | LEDS_YELLOW);
        led_state = 0;
        break;
    }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sink_mote_process, ev, data)
{
  static struct simple_udp_connection udp_conn;

  PROCESS_BEGIN();

 
  // Register UDP connection with callback
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_callback);

  PROCESS_END();
}
