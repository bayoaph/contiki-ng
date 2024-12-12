#include "contiki.h"
#include "dev/sht25.h"
#include "simple-udp.h"
#include <stdio.h>
#include "net/ipv6/uiplib.h"

#define SEND_INTERVAL (CLOCK_SECOND * 15)  // Send data every 15 seconds
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // Link-local all nodes multicast address

static struct simple_udp_connection udp_conn;

struct temperatureMessage {
    uint16_t temperature;
};

PROCESS(source_mote_process, "Source Mote Process");
AUTOSTART_PROCESSES(&source_mote_process);

static void send_temperature_data(void) {
    uip_ipaddr_t broadcast_addr;

    // Initialize the broadcast address (link-local all nodes multicast)
    uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

    int16_t temp = sht25.value(SHT25_VAL_TEMP); // Read temperature

    if (temp == -1) {
        printf("Error reading temperature sensor\n");
    } else {
        struct temperatureMessage tm;
        tm.temperature = temp;
	
	    simple_udp_sendto(&udp_conn, &tm, sizeof(tm), &broadcast_addr);

        int temperature_celsius = temp / 100;
        int temperature_fraction = temp % 100;

        if (temperature_fraction < 0) {
            temperature_fraction = -temperature_fraction;
        }

        printf("Sent temperature: %d.%02dC\n", temperature_celsius, temperature_fraction);

    }
}

PROCESS_THREAD(source_mote_process, ev, data)
{
    PROCESS_BEGIN();
    
    SENSORS_ACTIVATE(sht25);
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

    static struct etimer et;
    etimer_set(&et, SEND_INTERVAL);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        send_temperature_data();  // Send temperature data every 15 seconds
        etimer_reset(&et);
    }

    PROCESS_END();
}
