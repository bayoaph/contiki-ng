#include "contiki.h"
#include "simple-udp.h"
#include "net/netstack.h"
#include "net/ipv6/uiplib.h"
#include "arch/dev/sensor/sht11/sht11-sensor.h"
#include <stdio.h>

#define SEND_INTERVAL (CLOCK_SECOND * 15)
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1"

static struct simple_udp_connection udp_conn;

struct tempMessage {
    uint16_t temperature;
};

static void send_temperature(void) {
    uip_ipaddr_t broadcast_addr;
    uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

    int tmp = sht11_sensor.value(SHT11_SENSOR_TEMP);

    struct tempMessage temp;
    temp.temperature = tmp;

    // Send temperature data over UDP to the broadcast address
    simple_udp_sendto(&udp_conn, &tmp, sizeof(temp), &broadcast_addr);

    // Debug
    printf("Sent Temperature Data: %d.%dC\n", tmp / 100, tmp % 100);
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
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et)); // 15 seconds interval
        send_temperature(); // Send the temperature data
        etimer_reset(&et);
    }

    PROCESS_END();
}