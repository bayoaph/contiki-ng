#include "contiki.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>

#define UDP_PORT 1234

// Define the temperature message struct
struct temperatureMessage {
    uint16_t temperature;
};

static struct simple_udp_connection udp_conn;
static int data_counter = 0;

// Callback for receiving UDP data
static void udp_rx_callback(struct simple_udp_connection *c,
                             const uip_ipaddr_t *sender_addr,
                             uint16_t sender_port,
                             const uip_ipaddr_t *receiver_addr,
                             uint16_t receiver_port,
                             const uint8_t *data,
                             uint16_t datalen) {
    struct temperatureMessage received_data;
    memcpy(&received_data, data, sizeof(received_data));

    // Toggle LED color based on data_counter
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

    int16_t temperature = received_data.temperature;
    int integer_part = temperature / 10;
    int fractional_part = temperature % 10;

    printf("Received temperature: %d.%dC\n", integer_part, fractional_part);
}

PROCESS(sink_mote_process, "Sink Mote");
AUTOSTART_PROCESSES(&sink_mote_process);

PROCESS_THREAD(sink_mote_process, ev, data) {
    PROCESS_BEGIN();

    // Set up the UDP connection with the callback function
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

    while (1) {
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}