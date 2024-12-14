/*Kent Dico, IOT FINAL EXAM 2024. Target=z1 */

#include "contiki.h"
#include "simple-udp.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For abs() function

#define UDP_PORT 1234

/* Struct for temperature data */
typedef struct {
    int16_t temperature;
} temperature_msg_t;

static struct simple_udp_connection udp_conn;
static int led_state_counter = 0;

/* Callback function for received UDP packets */
static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen) {
    if (datalen != sizeof(temperature_msg_t)) {
        printf("Error: Received data length mismatch\n");
        return;
    }

    // Extract received temperature data
    temperature_msg_t temp_msg;
    memcpy(&temp_msg, data, sizeof(temp_msg));

    int temperature_celsius = temp_msg.temperature / 100;
    int temperature_fraction = abs(temp_msg.temperature % 100); // Ensure fraction is positive

    printf("Received temperature: %d.%02d°C from ", temperature_celsius, temperature_fraction);
    printf("node ");
    for (int i = 0; i < sizeof(sender_addr->u8); i++) {
        printf("%02x", sender_addr->u8[i]);
        if (i < sizeof(sender_addr->u8) - 1) printf(":");
    }
    printf("\n");

    // Toggle LEDs (Red -> Yellow -> Green)
    leds_off(LEDS_ALL);
    switch (led_state_counter % 3) {
        case 0:
            leds_on(LEDS_RED);
            break;
        case 1:
            leds_on(LEDS_YELLOW);
            break;
        case 2:
            leds_on(LEDS_GREEN);
            break;
    }
    led_state_counter++;
}

PROCESS(sink_process, "Sink Node Process");
AUTOSTART_PROCESSES(&sink_process);

PROCESS_THREAD(sink_process, ev, data) {
    PROCESS_BEGIN();

    // Register UDP connection with callback
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

    while (1) {
        PROCESS_WAIT_EVENT();  // Wait for incoming data
    }

    PROCESS_END();
}

/* Observation:
 * Sink receives temperature data from all source nodes and logs the temperature with the node's ID.
 * Temperatures are formatted correctly in the logs (e.g., 25.00°C).
 * LEDs alternate properly (red, yellow, green) with each received packet.
 * Data is being processed without errors or delays.
 * The sink is working as expected and integrates smoothly with the source nodes.
 */