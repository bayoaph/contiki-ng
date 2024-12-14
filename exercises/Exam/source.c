#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "dev/sensor/sht11/sht11-sensor.h"
#include <stdio.h>
#include "sys/node-id.h"
#include <stdlib.h>             
#include "net/ipv6/uip-debug.h"

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1"

static struct simple_udp_connection udp_conn;


void print_temperature_int_to_float(uint16_t temp)
{
  printf("%u.%uC\n", temp / 1000, temp % 1000);
}

struct temperatureMessage
{
  char messageString[50];
  uint16_t temperature;
};

static void send_broadcast_packet(struct temperatureMessage *payload)
{
  uip_ipaddr_t broadcast_addr;
  uint8_t buffer[sizeof(struct temperatureMessage)];
  memcpy(buffer, payload, sizeof(struct temperatureMessage)); // Serialize

  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
  simple_udp_sendto(&udp_conn, buffer, sizeof(struct temperatureMessage), &broadcast_addr);
}

static struct temperatureMessage tm;

PROCESS(source_process, "Source mote process");
AUTOSTART_PROCESSES(&source_process);

PROCESS_THREAD(source_process, ev, data)
{
  static struct etimer periodic_timer;
  static int16_t temp;
  PROCESS_BEGIN();

  /* Register the UDP connection */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

  etimer_set(&periodic_timer, CLOCK_SECOND * 15);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    /* Random temperature */
    temp = (rand() % 40000);
    printf("Sending temperature: ");
    print_temperature_int_to_float(temp);
    printf(" to ");
    tm.temperature = temp;
    sprintf(tm.messageString, "Node %u", node_id);
    send_broadcast_packet(&tm);

    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}
