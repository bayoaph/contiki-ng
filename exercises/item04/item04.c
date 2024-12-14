#include "contiki.h"
#include "net/ipv6/uip.h"
#include "net/ipv6/uip-udp-packet.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "node-id.h"
#include <stdio.h>
#include <string.h>

#define UDP_PORT 1234
#define SEND_INTERVAL (CLOCK_SECOND * 5) // Interval between sends

static struct uip_udp_conn *udp_conn;

struct timeMessage {
  clock_time_t time;
  uint16_t originator;
};

static struct timeMessage tmSent;
static struct timeMessage tmReceived;

static struct ctimer leds_off_timer_send;
static void timerCallback_turnOffLeds(void *ptr) {
  leds_off(LEDS_BLUE);
}

/*---------------------------------------------------------------------------*/
PROCESS(udp_rtt_process, "UDP RTT Process");
AUTOSTART_PROCESSES(&udp_rtt_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_rtt_process, ev, data) {
  PROCESS_BEGIN();

  static struct etimer send_timer;

  // Create UDP connection
  udp_conn = udp_new(NULL, UIP_HTONS(UDP_PORT), NULL);
  if (!udp_conn) {
    printf("Node %u: Failed to create UDP connection\n", node_id);
    PROCESS_EXIT();
  }
  udp_bind(udp_conn, UIP_HTONS(UDP_PORT));

  printf("Node %u: UDP connection bound, listening on port %d\n", node_id, UDP_PORT);

  etimer_set(&send_timer, SEND_INTERVAL);

  while (1) {
    PROCESS_WAIT_EVENT();

    if (etimer_expired(&send_timer)) {
      // Prepare and send timestamp
      tmSent.time = clock_time();
      tmSent.originator = node_id;

      printf("Node %u: Sending timestamp %lu\n", node_id, (unsigned long)tmSent.time);

      // Broadcast the packet to all nodes (link-local multicast)
      uip_ipaddr_t dest_ipaddr;
      uip_create_linklocal_allnodes_mcast(&dest_ipaddr);

      uip_udp_packet_sendto(udp_conn, &tmSent, sizeof(tmSent), &dest_ipaddr, UIP_HTONS(UDP_PORT));

      leds_on(LEDS_BLUE);
      ctimer_set(&leds_off_timer_send, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

      etimer_reset(&send_timer);
    }

    // Handle incoming packets
    if (ev == tcpip_event && uip_newdata()) {
      memcpy(&tmReceived, uip_appdata, sizeof(tmReceived));

      printf("Node %u: Received timestamp %lu from Node %u\n",
             node_id, (unsigned long)tmReceived.time, tmReceived.originator);

      if (tmReceived.originator == node_id) {
        // Calculate RTT
        clock_time_t rtt = clock_time() - tmReceived.time;
        printf("Node %u: RTT = %lu ms\n", node_id, (1000L * (unsigned long)rtt) / CLOCK_SECOND);
      } else {
        // Reply back to the originator
        uip_ipaddr_t reply_ipaddr = UIP_IP_BUF->srcipaddr;

        printf("Node %u: Sending reply back to Node %u\n", node_id, tmReceived.originator);
        uip_udp_packet_sendto(udp_conn, &tmReceived, sizeof(tmReceived), &reply_ipaddr, UIP_HTONS(UDP_PORT));
      }
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/* Exercise 4
 *
 * Compile and flash the above code to your Cooja simulator. Read and understand the code. Check
 * the contiki documentation for the unicast primitive and understand how the mechanism
 * with the callbacks works.
 *
 * Press the button and observe what happens. Have the receiver plugged in and see
 * what is printed out. The node where the USER button is pressed sends a packet containing
 * its current timestamp (in clock ticks, where 128 ticks = 1s) to the other.
 *
 * Your task: alter the program above such that the node where the USER button is pressed sends a
 * packet with its timestamp (is already done above) and THEN gets back a unicast packet with the
 * timestamp it has initially written into the first packet.
 */