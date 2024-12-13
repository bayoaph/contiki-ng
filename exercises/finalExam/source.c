#include "contiki.h"
#include <stdio.h>
#include <stdint.h>
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"
#include "net/ipv6/uiplib.h"
#include "../../main_contiki/arch/dev/sensor/sht11/sht11-sensor.h"

#define UDP_PORT 1234
#define SEND_INTERVAL (15 * CLOCK_SECOND)
#define BROADCAST_ADDR "ff02::1"

static struct simple_udp_connection udp_conn;

struct temperatureMessage
{
    uint16_t temperature;
};

PROCESS(source_mote_process, "Source Mote Process");
AUTOSTART_PROCESSES(&source_mote_process);

static void send_temperature_data(void)
{
    uip_ipaddr_t broadcast_addr;

    // Initialize the broadcast address
    uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
    int temp = sht11_sensor.value(SHT11_SENSOR_TEMP); // Read temperature

    if (temp == -1) {
        printf("Error reading temperature sensor\n");
    } else {
        struct temperatureMessage tm;
        tm.temperature = temp;

        simple_udp_sendto(&udp_conn, &tm, sizeof(tm), &broadcast_addr);
        printf("Temperature Sent: %d\n", temp);
    }
}

PROCESS_THREAD(source_mote_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize the UDP connection */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);
  SENSORS_ACTIVATE(sht11_sensor);

  while (1)
  {
    static struct etimer periodic_timer;
    etimer_set(&periodic_timer, SEND_INTERVAL);

    while (1){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        send_temperature_data();
        etimer_reset(&periodic_timer);
    }
  }
  PROCESS_END();
}