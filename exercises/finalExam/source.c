#include "contiki.h"
#include "simple-udp.h"
#include "net/netstack.h"
#include "net/ipv6/uiplib.h"
#include "arch/dev/sensor/sht11/sht11-sensor.h"
#include <stdio.h>

#define SEND_INTERVAL (CLOCK_SECOND * 15)  // send data every 15 seconds
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // link-local all nodes multicast address

static struct simple_udp_connection udp_conn;

// Define the temperature message struct
struct temperatureMessage {
    uint16_t temperature;
};

static void send_temp_data(void) {
    uip_ipaddr_t broadcast_addr;

    // Initialize the broadcast address
    uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

    // Read temperature from the sensor
    int temp = sht11_sensor.value(SHT11_SENSOR_TEMP);

    if (temp == -1) {
        printf("Error reading temperature sensor\n");
    } 
    else {
        struct temperatureMessage tm;
        tm.temperature = temp; // Store the temperature value in the struct

        // Send temperature data over UDP to the broadcast address
        simple_udp_sendto(&udp_conn, &tm, sizeof(tm), &broadcast_addr);

        int integer = temp / 100;
        int decimal = temp % 100;
        printf("Sent temperature: %d.%dC\n", integer, decimal);
    }
}

/*---------------------------------------------------------------------------*/
PROCESS(source_process, "Source Mote");
AUTOSTART_PROCESSES(&source_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(source_process, ev, data) {
    PROCESS_BEGIN();

    // Initialize the temperature sensor
    SENSORS_ACTIVATE(sht11_sensor);

    // Set up the UDP connection
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

    // Set periodic timer for sending data
    static struct etimer et;
    etimer_set(&et, SEND_INTERVAL);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et)); // Wait for the timer to expire
        send_temp_data(); // Send the temperature data
        etimer_reset(&et); // Reset the timer to send data again after the interval
    }

    PROCESS_END();
}
