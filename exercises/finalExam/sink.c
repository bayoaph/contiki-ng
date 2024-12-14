#include "contiki.h"
#include "net/netstack.h"
#include "simple-udp.h"
#include "leds.h"
#include <stdio.h>

#define UDP_PORT 1234

static struct simple_udp_connection udp_conn;
static uint8_t led_state = 0;  // LED state: 0 = Red, 1 = Yellow, 2 = Green

// Structure to hold the temperature data
struct temp_data {
    uint16_t temp_value;
};

// This function is called when UDP data is received
static void on_udp_data_received(struct simple_udp_connection *c,
                                 const uip_ipaddr_t *sender_addr,
                                 uint16_t sender_port,
                                 const uip_ipaddr_t *receiver_addr,
                                 uint16_t receiver_port,            
                                 const uint8_t *data,
                                 uint16_t datalen) 
{
    struct temp_data received_msg;
    memcpy(&received_msg, data, sizeof(received_msg));

    // Display the received temperature data
    printf("Received Temp: %d.%d°C\n", received_msg.temp_value / 100, received_msg.temp_value % 100);

    // Switch LED color every time data is received
    led_state = (led_state + 1) % 3;  // Cycle through 0, 1, 2 for different LED colors
    switch (led_state) {
        case 0:
            leds_on(LEDS_RED);    // Turn on red LED
            leds_off(LEDS_YELLOW | LEDS_GREEN);  // Turn off others
            break;
        case 1:
            leds_on(LEDS_YELLOW); // Turn on yellow LED
            leds_off(LEDS_RED | LEDS_GREEN);  // Turn off others
            break;
        case 2:
            leds_on(LEDS_GREEN);  // Turn on green LED
            leds_off(LEDS_RED | LEDS_YELLOW);  // Turn off others
            break;
    }
}

/*---------------------------------------------------------------------------*/
PROCESS(sink_node_process, "Sink Node Process");
AUTOSTART_PROCESSES(&sink_node_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(sink_node_process, ev, data) {
    PROCESS_BEGIN();

    // Register the UDP connection to receive data
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, on_udp_data_received);

    // Continuously listen for incoming UDP data
    while (1) {
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}
