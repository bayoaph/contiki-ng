#include "contiki.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h"


#define UDP_PORT 1234


// Define the temperature message struct
struct temperatureMessage {
    uint16_t temperature;
};


static struct simple_udp_connection udp_conn;
static int data_counter = 0;


// Function to convert IPv6 address to string format (for debugging if needed)
void ipaddr_to_str(char *str, uip_ipaddr_t *addr) {
    snprintf(str, 40, "%x:%x:%x:%x:%x:%x:%x:%x",
             uip_ntohs(addr->u16[0]), uip_ntohs(addr->u16[1]),
             uip_ntohs(addr->u16[2]), uip_ntohs(addr->u16[3]),
             uip_ntohs(addr->u16[4]), uip_ntohs(addr->u16[5]),
             uip_ntohs(addr->u16[6]), uip_ntohs(addr->u16[7]));
}


// Callback for receiving UDP data
static void udp_rx_callback(struct simple_udp_connection *c,
                             const uip_ipaddr_t *sender_addr,
                             uint16_t sender_port,
                             const uip_ipaddr_t *receiver_addr,
                             uint16_t receiver_port,
                             const uint8_t *data,
                             uint16_t datalen) {
    if (datalen != sizeof(struct temperatureMessage)) {
        printf("Received data length mismatch\n");
        return;
    }


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


    // Extract temperature and split into integer and fractional parts
    int16_t temperature = received_data.temperature;
    int temperature_celsius = temperature / 10;  // Integer part
    int temperature_fraction = temperature % 10;  // Fractional part


    // Get sender's node ID from the last byte of the sender's IP address
    // Assuming node IDs are mapped to the last byte of the sender's IPv6 address
    int sender_node_id = sender_addr->u8[15] % 10;  // Use the last byte, modulo 10 for 10 nodes


    // Print the message with the sender's temperature and node ID
    printf("Temperature Sink Node %d Received: 'Temp' message with value: %d.%02dC from node ID: %d\n", 
          node_id, temperature_celsius, temperature_fraction, sender_node_id);
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
