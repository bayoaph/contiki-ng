#include "contiki.h"
#include "simple-udp.h"
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

// Update the callback signature to match the expected one
static void udp_rx_callback(struct simple_udp_connection *c,
                             const uip_ipaddr_t *sender_addr,
                             uint16_t sender_port,
                             const uip_ipaddr_t *receiver_addr,
                             uint16_t receiver_port,            
                             const uint8_t *data,
                             uint16_t datalen) 
{
    struct temperatureMessage received_data;
    memcpy(&received_data, data, sizeof(received_data));

    // Toggle LED color based on data_counter
    data_counter++;
    if (data_counter % 3 == 0) {
        leds_on(LEDS_RED);
        leds_off(LEDS_YELLOW | LEDS_GREEN);
    } else if (data_counter % 3 == 1) {
        leds_on(LEDS_YELLOW);
        leds_off(LEDS_RED | LEDS_GREEN);
    } else {
        leds_on(LEDS_GREEN);
        leds_off(LEDS_RED | LEDS_YELLOW);
    }

    int16_t temperature = received_data.temperature;

    int integer_part = temperature / 100;
    int decimal_part = temperature % 100;

    if (decimal_part < 0) {
        decimal_part = -decimal_part;
    }
    
    printf("Received temperature: %d.%02dC\n", integer_part, decimal_part);
    
}

PROCESS(sink_station_process, "Sink Station Process");
AUTOSTART_PROCESSES(&sink_station_process);

PROCESS_THREAD(sink_station_process, ev, data)
{
    PROCESS_BEGIN();

    // Register the UDP connection with the updated callback function
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

    while (1) {
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}
