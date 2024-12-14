#include "contiki.h"
#include "dev/sht25.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include <stdio.h>

#define SEND_INTERVAL (CLOCK_SECOND * 15)  // Send data every 15 seconds
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // Link-local all nodes multicast address

static struct simple_udp_connection udp_conn;

// Define the temperature message struct
struct temperatureMessage {
    uint16_t temperature;  // Temperature in tenths of a degree
};

PROCESS(source_mote_process, "Source Mote");
AUTOSTART_PROCESSES(&source_mote_process);

// Function to send the temperature data over UDP
static void send_temperature_data(void) {
    uip_ipaddr_t broadcast_addr;

    // Convert the broadcast address from string to IP address format
    if (!uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr)) {
        printf("Error: Invalid broadcast address\n");
        return;
    }

    // Read temperature data from the SHT25 sensor
    int temp = sht25.value(SHT25_VAL_TEMP);  // Temperature in hundredths of a degree Celsius

    // Check for sensor read errors
    if (temp == -1) {
        printf("Error: Failed to read temperature from sensor\n");
    } else {
        struct temperatureMessage tm;
        tm.temperature = temp;

        // Send the temperature data over UDP
        simple_udp_sendto(&udp_conn, &tm, sizeof(tm), &broadcast_addr);

        // Convert the temperature to integer and fractional parts for display
        int integer_part = temp / 10;
        int fractional_part = temp % 10;
        printf("Sent temperature: %d.%dC\n", integer_part, fractional_part);
    }
}

PROCESS_THREAD(source_mote_process, ev, data) {
    PROCESS_BEGIN();

    // Activate the SHT25 temperature sensor
    SENSORS_ACTIVATE(sht25);

    // Register the UDP connection for communication
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

    // Set up the periodic timer for sending data
    static struct etimer et;
    etimer_set(&et, SEND_INTERVAL);

    while (1) {
        // Wait for the timer to expire and send temperature data
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        send_temperature_data();

        // Reset the timer for the next send
        etimer_reset(&et);
    }

    PROCESS_END();
}
