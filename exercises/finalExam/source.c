#include "contiki.h"
#include "dev/sht25.h"
#include "simple-udp.h"
#include "net/ipv6/uiplib.h"
#include <stdio.h>
#include <string.h>

#define TRANSMISSION_INTERVAL (CLOCK_SECOND * 15) // Transmission interval
#define UDP_PORT 1234                              // UDP communication port
#define MULTICAST_ADDRESS "ff02::1"               // IPv6 multicast address

typedef struct __attribute__((__packed__)) {
    uint16_t temp_value; // Temperature in hundredths of a degree
} TempMsg;

static struct simple_udp_connection udp_connection;

PROCESS(temperature_broadcast_process, "Temperature Broadcast Process");
AUTOSTART_PROCESSES(&temperature_broadcast_process);

static void send_temperature_packet(void) {
    uip_ipaddr_t multicast_address;

    if (!uiplib_ipaddrconv(MULTICAST_ADDRESS, &multicast_address)) {
        printf("Error: Invalid multicast address\n");
        return;
    }

    int16_t raw_temperature = sht25.value(SHT25_VAL_TEMP);
    if (raw_temperature == -1) {
        printf("Sensor error: Unable to fetch temperature\n");
        return;
    }

    TempMsg msg;
    msg.temp_value = uip_htons(raw_temperature);

    simple_udp_sendto(&udp_connection, &msg, sizeof(msg), &multicast_address);

    int integer_part = raw_temperature / 100;
    int fractional_part = raw_temperature % 100;
    fractional_part = fractional_part < 0 ? -fractional_part : fractional_part;

    printf("Broadcasted Temperature: %d.%02d°C\n", integer_part, fractional_part);
}

PROCESS_THREAD(temperature_broadcast_process, ev, data) {
    static struct etimer periodic_timer;

    PROCESS_BEGIN();

    SENSORS_ACTIVATE(sht25);
    simple_udp_register(&udp_connection, UDP_PORT, NULL, UDP_PORT, NULL);

    etimer_set(&periodic_timer, TRANSMISSION_INTERVAL);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        send_temperature_packet();
        etimer_reset(&periodic_timer);
    }

    SENSORS_DEACTIVATE(sht25);

    PROCESS_END();
}
