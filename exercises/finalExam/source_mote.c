#define SEND_INTERVAL (CLOCK_SECOND * 15)
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1"

#include "contiki.h"
#include <stdio.h>
#include <string.h>
#include "dev/temperature-sensor.h"
#include "arch/dev/sensor/sht11/sht11-sensor.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"

static struct simple_udp_connection udp_connection;

struct temp_message {
    char type[5]; // Message type
    int value;    // Temperature
};

static void broadcast_temp(void) {
    static int temp;
    uip_ipaddr_t broadcast_addr;

    if (!uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr)) {
    printf("Error: Invalid broadcast address\n");
    return;
    }

    temp = sht11_sensor.value(0); // Read temp value from sensor
    printf("Temperature Broadcast: Sending temperature: %d\n", temp);

    struct temp_message temp_msg;
    snprintf(temp_msg.type, sizeof(temp_msg.type), "Temp");
    temp_msg.value = temp;

    // Send message to the broadcast address
    simple_udp_sendto(&udp_connection, &temp_msg, sizeof(temp_msg), &broadcast_addr);
}

PROCESS(temp_broadcast_process, "Source Mote Process");
PROCESS_THREAD(temp_broadcast_process, ev, data) {
    static struct etimer periodic_timer;

    PROCESS_BEGIN();

    SENSORS_ACTIVATE(sht11_sensor);

    // Register connection
    simple_udp_register(&udp_connection, UDP_PORT, NULL, UDP_PORT, NULL);

    while (1) {
        etimer_set(&periodic_timer, SEND_INTERVAL);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        broadcast_temp();
    }

    SENSORS_DEACTIVATE(sht11_sensor);

    PROCESS_END();
}

AUTOSTART_PROCESSES(&temp_broadcast_process);