/*Kent Dico, IOT FINAL EXAM 2024. Target=z1 */

#include "contiki.h"
#include "dev/sht25.h"  
#include "simple-udp.h"
#include <stdio.h>
#include "net/ipv6/uiplib.h"
#include <stdlib.h> 

#define SEND_INTERVAL (CLOCK_SECOND * 15)  // Send data every 15 seconds
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" 

/* Struct for temperature data */
typedef struct {
    int16_t temperature;
} temperature_msg_t;

static struct simple_udp_connection udp_conn;

PROCESS(source_process, "Source Node Process");
AUTOSTART_PROCESSES(&source_process);

static void send_temperature_data(void) {
    uip_ipaddr_t broadcast_addr;
    uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

    int16_t raw_temp = sht25.value(SHT25_VAL_TEMP);

    if (raw_temp == -1) {
        // Simulate temperature if sensor data is unavailable
        printf("Error: Failed to read temperature sensor. Sending simulated value.\n");
        raw_temp = (rand() % 6000) - 2000; // Random temperature between -20.00°C and 40.00°C
    }

    // Prepare and send the temperature message
    temperature_msg_t temp_msg;
    temp_msg.temperature = raw_temp;
    simple_udp_sendto(&udp_conn, &temp_msg, sizeof(temp_msg), &broadcast_addr);

    // Print temperature in a readable format
    int temperature_celsius = raw_temp / 100;
    int temperature_fraction = abs(raw_temp % 100); // Ensure fraction is positive
    printf("Sent temperature: %d.%02d°C\n", temperature_celsius, temperature_fraction);
}

PROCESS_THREAD(source_process, ev, data) {
    static struct etimer timer;

    PROCESS_BEGIN();

    // Activate the temperature sensor
    SENSORS_ACTIVATE(sht25);

    // Register the UDP connection
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

    // Set periodic timer for sending data
    etimer_set(&timer, SEND_INTERVAL);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

        send_temperature_data();  // Send temperature data
        etimer_reset(&timer);     // Reset the timer
    }

    PROCESS_END();
}

/* Observation:
 * Source nodes send temperature data every 15 seconds as expected.
 * If the sensor doesn’t work, a random temperature between -20°C and 40°C is sent instead.
 * Sent temperatures appear in the logs in a readable format like 25.00°C.
 * Multicast (ff02::1) works perfectly for broadcasting to all nodes.
 * System works well, but actual sensor readings would improve accuracy instead of random values.
 */
