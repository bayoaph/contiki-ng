#include "contiki.h"
#include <stdio.h>
#include "etimer.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h"
#include "simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "net/ipv6/uip-debug.h"


#define UDP_PORT 1234

struct timeMessage
{
  clock_time_t time;
  unsigned short originator;
};

// Declare necessary structures
static struct timeMessage tmReceived;
static struct timeMessage tmSent;

// void print_temperature_binary_to_float(uint16_t temp)
// {
//   printf("%d.%d", (temp / 10 - 396) / 10, (temp / 10 - 396) % 10);
// }

// static struct ctimer leds_off_timer_send;

static struct simple_udp_connection udp_conn;

// Timer callback to turn off the blue LED
// static void timerCallback_turnOffLeds()
// {
//   leds_off(LEDS_BLUE);
// }

// Callback function to handle incoming packets
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen)
{

  memcpy(&tmReceived, data, sizeof(struct timeMessage)); // Deserialize received data
  
  printf("Packet received from ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\n");
  printf("time received = %lu clock ticks = %lu secs %lu millis\n",
         (unsigned long)tmReceived.time,
         (unsigned long)tmReceived.time / CLOCK_SECOND,
         (1000L * ((unsigned long)tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND);
  printf("originator = %u\n", tmReceived.originator);

  // If the packet received is not from the current node, send it back
  if (tmReceived.originator != node_id)
  {
    simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
    printf("Sending packet back to originator: %u\n", tmReceived.originator);
  }
  else
  {
    // Round-trip time calculation
    clock_time_t rtt = clock_time() - tmReceived.time;
    printf("RTT = %lu ms\n", (rtt * 1000) / CLOCK_SECOND);
  }
}

/*---------------------------------------------------------------------------*/
// PROCESS(example_unicast_process, "RTT using Rime Unicast Primitive");
PROCESS(example_unicast_process, "RTT using Simple UDP");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/

// static void recv_uc(struct unicast_conn *c, const rimeaddr_t *from);
// static const struct unicast_callbacks unicast_callbacks = {recv_uc};
// static struct unicast_conn uc;

/* specify the address of the unicast */
// static rimeaddr_t addr;

PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_BEGIN();

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);
  SENSORS_ACTIVATE(button_sensor);

  uip_ipaddr_t addr;

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    // Populate the timestamp and originator ID
    tmSent.time = clock_time();
    tmSent.originator = node_id;

    // Dynamically determine the destination node's IPv6 address
    if (node_id % 2 == 0)
    {
      uip_create_linklocal_allnodes_mcast(&addr);
    }
    else
    {
      uip_create_linklocal_allnodes_mcast(&addr);
    }

    // Send the packet
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
