#include "contiki.h"
#include "arch/dev/sensor/sht11/sht11-sensor.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include <stdio.h>

#define SEND_INTERVAL (CLOCK_SECOND * 15)  // Send data every 15 seconds
#define UDP_PORT 7777  // UDP port for communication
#define BROADCAST_ADDR "ff02::1"  // Link-local all nodes multicast address

static struct simple_udp_connection udp_conn;

// Define the temperature message struct (float)
struct temperatureMessage {
    float temperature;  // Temperature value in Celsius
};

PROCESS(source_mote_process, "Source Mote");
AUTOSTART_PROCESSES(&source_mote_process);

// Function to send temperature data over UDP
static void send_temperature_data(void) {
    uip_ipaddr_t broadcast_addr;
    int temp = sht11_sensor.value(SHT11_SENSOR_TEMP);  // Read temperature in tenths of degrees Celsius
    // Convert the temperature to Celsius (as a float)
    float temperature_celsius = temp / 10.0;

    struct temperatureMessage tm;
    tm.temperature = temperature_celsius;  // Store the temperature in Celsius

    // Convert multicast address string to uip_ipaddr_t
    uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

    // Send temperature data over UDP
    simple_udp_sendto(&udp_conn, &tm, sizeof(tm), &broadcast_addr);
    double temperature = (double)temperature_celsius;
    printf("Sent temperature: %.2fC\n", temperature);
    
}

PROCESS_THREAD(source_mote_process, ev, data) {
    PROCESS_BEGIN();

    // Initialize the temperature sensor
    SENSORS_ACTIVATE(sht11_sensor);

    // Set up the UDP connection
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

    // Set periodic timer for sending data
    static struct etimer et;
    etimer_set(&et, SEND_INTERVAL);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        send_temperature_data();
        etimer_reset(&et);
    }

    PROCESS_END();
}
