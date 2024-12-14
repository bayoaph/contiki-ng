#include "contiki.h"
#include "dev/leds.h"
#include "net/ipv6/simple-udp.h"
#include <stdio.h>
#include <string.h>

#define UDP_PORT 7777 // UDP port for communication

static struct simple_udp_connection udp_conn;

// LED states
static uint8_t led_state = 0;

/**
 * @brief Callback function for handling received UDP messages.
 *
 * @param c UDP connection instance.
 * @param sender_addr Address of the sender.
 * @param receiver_addr Address of the receiver.
 * @param data Pointer to the received data.
 * @param datalen Length of the received data.
 */
static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen) {

    // Parse temperature from received data (float)
    float temperature;
    memcpy(&temperature, data, sizeof(float));

    // Print received temperature with 2 decimal places
    double temp = (double)temperature;
    printf("Temperature received: %.2fC\n", temp);

    // Toggle LEDs to indicate activity (Red -> Yellow -> Green)
    leds_off(LEDS_ALL);
    if (led_state == 0) {
        leds_on(LEDS_RED);
    } else if (led_state == 1) {
        leds_on(LEDS_YELLOW);
    } else if (led_state == 2) {
        leds_on(LEDS_GREEN);
    }
    led_state = (led_state + 1) % 3;  // Cycle LED states (0 -> 1 -> 2 -> 0)
}

/*---------------------------------------------------------------------------*/
PROCESS(sink_mote_process, "Sink Mote Process");
AUTOSTART_PROCESSES(&sink_mote_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(sink_mote_process, ev, data) {
    PROCESS_BEGIN();

    // Initialize UDP connection
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

    while (1) {
        PROCESS_WAIT_EVENT(); // Wait indefinitely for incoming events
    }

    PROCESS_END();
}
