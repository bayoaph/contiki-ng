#include <stdio.h>
#include <stdlib.h>
#include "contiki.h"
#include "contiki-net.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h"

#define UDP_SERVER_PORT 1234
#define UDP_CLIENT_PORT 4321

static struct simple_udp_connection udp_conn;
static uip_ipaddr_t dest_ipaddr;

PROCESS(source_process, "Source Process");
AUTOSTART_PROCESSES(&source_process);

PROCESS_THREAD(source_process, ev, data) {
  static struct etimer timer;
  static int temperature = 2500; // Example: 25.00°C

  PROCESS_BEGIN();

  // Set the destination IP address (sink mote's IP)
  uip_ip6addr(&dest_ipaddr, 0xfe80, 0, 0, 0, 0x20b, 0xb, 0xb, 0xb); 

  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, &dest_ipaddr, UDP_SERVER_PORT, NULL);

  printf("Processing. Sending data");
  uip_debug_ipaddr_print(&dest_ipaddr);
  printf("\n");

  // Set a timer to send data every 15 seconds
  etimer_set(&timer, CLOCK_SECOND * 15);

  while (1) {
    PROCESS_YIELD();  // Yield to the event system and wait for an event

    if (ev == PROCESS_EVENT_TIMER && data == &timer) {
      // Simulate temperature fluctuation
      temperature += (rand() % 3) - 1;

      // Format temperature as a string
      char temp_str[16];
      snprintf(temp_str, sizeof(temp_str), "Temp:%d.%02d", temperature / 100, temperature % 100);

      // Send temperature data to sink
      printf("Sending temperature: %s to ", temp_str);
      uip_debug_ipaddr_print(&dest_ipaddr);
      printf("\n");

      simple_udp_sendto(&udp_conn, temp_str, strlen(temp_str), &dest_ipaddr);

      // Reset the timer to continue the loop
      etimer_reset(&timer);
    }
  }

  PROCESS_END();
}
