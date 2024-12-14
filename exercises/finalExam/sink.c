#include "contiki.h"
#include "net/netstack.h"
#include "simple-udp.h"
#include "leds.h"
#include <stdio.h>

#define UDP_PORT 1234

static struct simple_udp_connection udp_conn;
static uint8_t led_color = 0;  // LED color: 0 = Red, 1 = Yellow, 2 = Green

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

    //struct temperatureMessage *msg = (struct temperatureMessage *)data;
    printf("Received temperature: %d.%dC\n", msg.temperature / 100, msg.temperature % 100);

    // Toggle LED color every time data is received
    led_color = (led_color + 1) % 3;  // Cycle through 0, 1, 2
    switch (led_color) {
        case 0:
            leds_on(LEDS_RED);   // Red LED
            leds_off(LEDS_YELLOW | LEDS_GREEN);
            break;
        case 1:
            leds_on(LEDS_YELLOW); // Yellow LED
            leds_off(LEDS_RED | LEDS_GREEN);
            break;
        case 2:
            leds_on(LEDS_GREEN);  // Green LED
            leds_off(LEDS_RED | LEDS_YELLOW);
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