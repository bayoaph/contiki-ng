/* Modified by Kent Dico, 2024. Target=z1 */

#include "contiki.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h"
#include "sys/rtimer.h"
#include <stdio.h>

#define UDP_PORT 1234

struct timeMessage {
  clock_time_t time;
  uint16_t originator;
};

static struct simple_udp_connection udp_conn;

/* Structs for sending and receiving data */
static struct timeMessage tmReceived;
static struct timeMessage tmSent;

static struct ctimer ctimer;

/*---------------------------------------------------------------------------*/
/* Timer callback to turn off the LED */
static void timerCallback_turnOffLeds(void *ptr) {
  leds_off(LEDS_RED);
  printf("LED turned off (ctimer callback).\n");
}

/*---------------------------------------------------------------------------*/
/* Callback function for receiving packets */
static void udp_recv_uc(struct simple_udp_connection *c,
                        const uip_ipaddr_t *sender_addr,
                        uint16_t sender_port,
                        const uip_ipaddr_t *receiver_addr,
                        uint16_t receiver_port,
                        const uint8_t *data,
                        uint16_t datalen) {
  clock_time_t rtt = clock_time();

  /* Copy the received data into tmReceived */
  memcpy(&tmReceived, data, sizeof(tmReceived));

  /* Turn on LED to indicate reception */
  leds_on(LEDS_RED);
  ctimer_stop(&ctimer); /* Stop any existing timer */
  ctimer_set(&ctimer, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

  /* Print received packet information */
  printf("Packet received from ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\n");
  printf("Time received = %lu clock ticks = %lu secs %lu millis\n",
         (unsigned long)tmReceived.time,
         (unsigned long)tmReceived.time / CLOCK_SECOND,
         (1000L * ((unsigned long)tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND);
  printf("Originator = %u\n", tmReceived.originator);

  /* If the packet is not ours, send it back to the originator */
  if (tmReceived.originator != node_id) {
    printf("Sending back the received timestamp\n");
    simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
  } else {
    /* Packet has completed a round-trip, calculate RTT */
    rtt -= tmReceived.time;
    printf("RTT = %lu ms\n", (1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "RTT using Simple UDP");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_unicast_process, ev, data) {
  PROCESS_BEGIN();

  /* Initialize LEDs */
  leds_init();

  /* Initialize Simple UDP */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_uc);

  /* Activate the button sensor */
  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* Prepare the timeMessage struct */
    tmSent.time = clock_time();
    tmSent.originator = node_id;

    /* Turn on LED to indicate sending */
    leds_on(LEDS_RED);
    ctimer_stop(&ctimer); /* Stop any existing timer */
    ctimer_set(&ctimer, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

    /* Prepare destination address */
    uip_ipaddr_t addr;
    if (node_id % 2 == 0) {
      uip_create_linklocal_allnodes_mcast(&addr); /* Multicast for paired node */
    } else {
      uip_create_linklocal_allnodes_mcast(&addr); /* Multicast for paired node */
    }

    /* Send the packet */
    simple_udp_sendto(&udp_conn, &tmSent, sizeof(tmSent), &addr);
    printf("Sending packet to ");
    uip_debug_ipaddr_print(&addr);
    printf("\n");
  }

  SENSORS_DEACTIVATE(button_sensor);

  PROCESS_END();
}

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

/* Changes:
 * Replaced Rime communication with Simple UDP for unicast communication.
 * Added simple_udp_register() to initialize the UDP connection.
 * Used simple_udp_sendto() for sending packets to a multicast or unicast address.
 * Added uip_debug_ipaddr_print() for printing IP addresses.
 * Replaced packet addressing logic with uip_create_linklocal_allnodes_mcast() for multicast.
 * Added RTT calculation when the originator receives its own packet back.
 * Simplified packet handling using memcpy() to copy data between structs.
 * Replaced LEDs and legacy structures with updated Contiki-NG functions.
 */

/* Observations:
 * Pressing the button sends a unicast packet with a timestamp.
 * Receiving node processes the packet and sends it back if it's not the originator.
 * Sender receives the echoed packet and calculates RTT.
 * Logs show packet sending, receiving, and RTT calculation.
 * Nodes correctly handle and respond to packets.
 */

