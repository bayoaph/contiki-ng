#include "contiki.h"
#include "dev/sht25.h"
#include "simple-udp.h"
#include "net/ipv6/uiplib.h"
#include <stdio.h>
#include <string.h>

#define TRANSMISSION_INTERVAL (CLOCK_SECOND * 15) // Transmission interval
#define UDP_COMM_PORT 1234                         // UDP communication port
#define MULTICAST_ADDRESS "ff02::1"              // IPv6 multicast address

// Define the UDP connection
static struct simple_udp_connection udp_connection;

// Define the structure for temperature data
typedef struct {
    char message_type[8]; // Message identifier
    int16_t temp_value;   // Raw temperature value
} TemperaturePacket;

PROCESS(temperature_broadcast_process, "Temperature Broadcast Process");
AUTOSTART_PROCESSES(&temperature_broadcast_process);

// Function to prepare and send the temperature packet
static void send_temperature_packet(void) {
    uip_ipaddr_t multicast_address;

    // Convert multicast address to UIP format
    if (!uiplib_ipaddrconv(MULTICAST_ADDRESS, &multicast_address)) {
        printf("Error: Invalid multicast address\n");
        return;
    }

    // Fetch the raw temperature value
    int16_t raw_temperature = sht25.value(SHT25_VAL_TEMP);
    if (raw_temperature == -1) {
        printf("Sensor error: Unable to fetch temperature\n");
        return;
    }

    // Populate the temperature packet
    TemperaturePacket temp_packet;
    strncpy(temp_packet.message_type, "TEMP", sizeof(temp_packet.message_type));
    temp_packet.temp_value = raw_temperature;

    // Send the packet using UDP
    simple_udp_sendto(&udp_connection, &temp_packet, sizeof(temp_packet), &multicast_address);

    // Log the temperature in a human-readable format
    int integer_part = raw_temperature / 100;
    int fractional_part = raw_temperature % 100;
    fractional_part = fractional_part < 0 ? -fractional_part : fractional_part;

    printf("Broadcasted Temperature: %d.%02dC\n", integer_part, fractional_part);
}

PROCESS_THREAD(temperature_broadcast_process, ev, data) {
    static struct etimer periodic_timer;

    PROCESS_BEGIN();

    // Activate the SHT25 temperature sensor
    SENSORS_ACTIVATE(sht25);

    // Initialize the UDP connection
    simple_udp_register(&udp_connection, UDP_COMM_PORT, NULL, UDP_COMM_PORT, NULL);

    // Set the periodic timer
    etimer_set(&periodic_timer, TRANSMISSION_INTERVAL);

    while (1) {
        // Wait until the timer expires
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        // Transmit the temperature data
        send_temperature_packet();

        // Reset the timer for the next interval
        etimer_reset(&periodic_timer);
    }

    // Deactivate the sensor before ending the process
    SENSORS_DEACTIVATE(sht25);

    PROCESS_END();
}
