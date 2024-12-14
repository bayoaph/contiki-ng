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

struct timeMessage
{
  clock_time_t time;
  unsigned short originator;
};

/* two timeMessage struct declaration/instantiations */
static struct timeMessage tmReceived;
static struct timeMessage tmSent;

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "RTT using Simple UDP");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/

/* Callback function for receiving UDP packets */
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen)
{
  // Round-trip time, will be decremented later
  clock_time_t rtt = clock_time();

  /* Copy received data into tmReceived */
  memcpy(&tmReceived, data, sizeof(tmReceived));

  /* Print the contents of the received packet */
  printf("Packet received from ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\n");
  printf("time received = %lu clock ticks = %lu secs %lu millis\n",
         (unsigned long)tmReceived.time,
         (unsigned long)tmReceived.time / CLOCK_SECOND,
         (1000L * ((unsigned long)tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND);
  printf("originator = %u\n", tmReceived.originator);

  // If the packet received is not ours, send it back to the originator
  if (tmReceived.originator != node_id)
  {
    printf("Sending back the received timestamp\n");
    simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
  }
  else
  { // Our packet has completed a round-trip
    rtt -= tmReceived.time;
    printf("RTT = %lu ms\n", (1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize Simple UDP */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

  SENSORS_ACTIVATE(button_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* When the button is pressed, read the current time and write it to tmSent */
    tmSent.time = clock_time();
    tmSent.originator = node_id;

    /* Prepare the destination address (neighbor node) */
    uip_ipaddr_t addr;
    if (node_id % 2 == 0)
    {
      uip_create_linklocal_allnodes_mcast(&addr);
    }
    else
    {
      uip_create_linklocal_allnodes_mcast(&addr);
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
/*
ANSWER: Used the UDP library for unicasting instead of the Rime. 
Each node in the  simulation can send and receive messages over the network through the UDP connection.
Pressing the button on the node records the current time and includes it in a message sent to the next node 
Even sends to the next node, odd sends to the previous node
*/