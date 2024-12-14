#include "contiki.h"
#include "contiki-net.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h"
#include <stdio.h>
#include <stdlib.h>

#define UDP_SERVER_PORT 5678
#define UDP_CLIENT_PORT 8765

static struct simple_udp_connection udp_conn;
static uip_ipaddr_t dest_ipaddr;

PROCESS(source_process, "UDP Source Process");
AUTOSTART_PROCESSES(&source_process);

PROCESS_THREAD(source_process, ev, data) {
  static struct etimer periodic_timer;
  static int temperature = 25;

  PROCESS_BEGIN();

  // Set the sink's IPv6 address
  uip_ip6addr(&dest_ipaddr, 0xfe80, 0, 0, 0, 0x20b, 0xb, 0xb, 0xb); // Sink IP: fe80::20b:b:b:b

  // Initialize UDP connection
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, &dest_ipaddr, UDP_SERVER_PORT, NULL);

  printf("Source process started. Sending data to sink at ");
  uip_debug_ipaddr_print(&dest_ipaddr);
  printf("\n");

  // Set a periodic timer to send data every 15 seconds
  etimer_set(&periodic_timer, CLOCK_SECOND * 15);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_reset(&periodic_timer);

    // Simulate temperature change
    temperature += (rand() % 3) - 1;

    // Send temperature data to sink
    printf("Sending temperature: %d to ", temperature);
    uip_debug_ipaddr_print(&dest_ipaddr);
    printf("\n");

    char temp_str[16];
    snprintf(temp_str, sizeof(temp_str), "Temp:%d", temperature);
    simple_udp_sendto(&udp_conn, temp_str, strlen(temp_str), &dest_ipaddr);
  }

  PROCESS_END();
}
