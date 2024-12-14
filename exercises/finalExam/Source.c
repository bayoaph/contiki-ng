#include "contiki.h"
#include "dev/sht11/sht11-sensor.h"
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
    uint16_t temperature;
};


PROCESS(source_mote_process, "Source Mote");
AUTOSTART_PROCESSES(&source_mote_process);


static void send_temperature_data(void) {
    uip_ipaddr_t broadcast_addr;


    // Initialize the broadcast address
    uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);


    int temp = sht11_sensor.value(SHT11_SENSOR_TEMP); // Read temperature


    if (temp == -1) {
        printf("Error reading temperature sensor\n");
    } else {
        struct temperatureMessage tm;
        tm.temperature = temp;


        // Send temperature data over UDP
        simple_udp_sendto(&udp_conn, &tm, sizeof(tm), &broadcast_addr);


        int integer_part = temp / 10;
        int fractional_part = temp % 10;
        printf("Sent temperature: %d.%dC\n", integer_part, fractional_part);
    }
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




