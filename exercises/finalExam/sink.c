#include "contiki.h"
#include "net/netstack.h"
#include "simple-udp.h"
#include "leds.h"
#include <stdio.h>

#define UDP_PORT 1234

static struct simple_udp_connection udp_conn;
static uint8_t led_counter = 0;

// Define the temperature message struct
struct temperatureMessage {
    uint16_t temperature;
};

// Callback function to handle received temperature data
static void udp_recv_callback(struct simple_udp_connection *c,
                             const uip_ipaddr_t *sender_addr,
                             uint16_t sender_port,
                             const uip_ipaddr_t *receiver_addr,
                             uint16_t receiver_port,            
                             const uint8_t *data,
                             uint16_t datalen) 
{
    struct temperatureMessage msg;
    memcpy(&msg, data, sizeof(msg));

    printf("Received Temperature Data: %d.%dC\n", msg.temperature / 100, msg.temperature % 100);

    // Toggle LED
    switch (led_counter) {
        case 0: // Red LED
            leds_on(LEDS_RED);   
            leds_off(LEDS_YELLOW);
            leds_off(LEDS_GREEN);
            led_counter = 1;
            break;
        case 1: // Yellow LED
            leds_on(LEDS_YELLOW); 
            leds_off(LEDS_RED);
            leds_off(LEDS_GREEN);
            led_counter = 2;
            break;
        case 2: // Green LED
            leds_on(LEDS_GREEN);  
            leds_off(LEDS_RED);
            leds_off(LEDS_YELLOW);
            led_counter = 0;
            break;
    }
}

/*---------------------------------------------------------------------------*/
PROCESS(sink_process, "Sink Mote");
AUTOSTART_PROCESSES(&sink_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(sink_process, ev, data) {
    PROCESS_BEGIN();

    // Register the UDP connection
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

    while (1) {
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}