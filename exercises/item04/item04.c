
#include "contiki.h"
// #include "net/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h"
#include "sys/rtimer.h"
#include <stdio.h>
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h"

#define UDP_PORT 1234

struct timeMessage
{
  clock_time_t time;
  unsigned short originator;
};

static struct simple_udp_connection udp_conn;

/* two timeMessage struct declaration/instantiations */
static struct timeMessage tmReceived;
static struct timeMessage tmSent;

/* this function has been defined to be called when a unicast is being received */
static void udp_recv_uc(struct simple_udp_connection *c,
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
PROCESS(example_unicast_process, "RTT using Rime Unicast Primitive");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize Simple UDP */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_uc);

  SENSORS_ACTIVATE(button_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* when the button is pressed, read the current time and write it to the
     * previously declared tmSent struct */
    tmSent.time = clock_time();
    /* write the id of then node where the button is pressed into the packet */
    tmSent.originator = node_id;

    /* prepare the unicast packet to be sent. Write the contents of the struct, where we
     * have just written the time and the id into, to the packet we intend to send
     */
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