#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "net/linkaddr.h"
#include "net/ipv6/uip-debug.h"
#include "node-id.h"
// #include "sys/rtimer.h"
#include <stdio.h>

#define UDP_PORT 1234

struct timeMessage {
  clock_time_t time;
  unsigned short originator;
};

static struct timeMessage tmReceived;
static struct timeMessage tmSent;

static struct simple_udp_connection udp_conn;
static struct ctimer leds_off_timer_send;

/* Timer callback to turn off the  LED */
static void timerCallback_turnOffLeds() {
  leds_off(LEDS_BLUE);
}

void generate_ipv6_address(uip_ipaddr_t *addr, uint16_t node_id) {
  linkaddr_t target_linkaddr;

  // Use ternary operator for concise conditional assignment
  target_linkaddr.u8[0] = linkaddr_node_addr.u8[0];
  target_linkaddr.u8[2] = linkaddr_node_addr.u8[2];
  target_linkaddr.u8[4] = linkaddr_node_addr.u8[4];

  // Modify the address parts based on the node_id parity
  target_linkaddr.u8[1] = linkaddr_node_addr.u8[1] + (node_id % 2 == 0 ? 1 : -1);
  target_linkaddr.u8[3] = linkaddr_node_addr.u8[3] + (node_id % 2 == 0 ? -1 : 1);
  target_linkaddr.u8[5] = linkaddr_node_addr.u8[5] + (node_id % 2 == 0 ? -1 : 1);
  target_linkaddr.u8[6] = linkaddr_node_addr.u8[6] + (node_id % 2 == 0 ? -1 : 1);
  target_linkaddr.u8[7] = linkaddr_node_addr.u8[7] + (node_id % 2 == 0 ? -1 : 1);

  // Construct the IPv6 address
  uip_ip6addr(addr, 0xfe80, 0, 0, 0, 0x212,
              (target_linkaddr.u8[2] << 8) | target_linkaddr.u8[3],
              (target_linkaddr.u8[4] << 8) | target_linkaddr.u8[5],
              (target_linkaddr.u8[6] << 8) | target_linkaddr.u8[7]);
}

/* Callback function for received packets */
static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen) {
  clock_time_t rtt = clock_time();

  printf("Unicast message received from ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\n");

  leds_on(LEDS_BLUE);

  // Set timer to turn off LED after 1/8 second
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

  // Copy received data into `tmReceived`
  memcpy(&tmReceived, data, sizeof(tmReceived));

  // Print the received packet
  printf("Time received = %lu clock ticks", (unsigned long)tmReceived.time);
  printf(" = %lu secs ", (unsigned long)tmReceived.time / CLOCK_SECOND);
  printf("%lu millis ", (1000L * ((unsigned long)tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND);
  printf("Originator = %d\n", tmReceived.originator);

  if (tmReceived.originator != node_id) {
    // Send the packet back to the originator
    simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
    printf("Sending packet back to originator.\n");
  } else {
    // Calculate RTT
    rtt -= tmReceived.time;
    printf("RTT = %lu ms\n", (1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
  }
}

PROCESS(example_unicast_process, "RTT using Simple UDP");
AUTOSTART_PROCESSES(&example_unicast_process);

PROCESS_THREAD(example_unicast_process, ev, data) {
  static uip_ipaddr_t addr;

  PROCESS_BEGIN();

  // Initialize UDP connection
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    // Prepare the packet to send
    tmSent.time = clock_time();
    tmSent.originator = node_id;

    // Construct the neighbor's IPv6 address
    generate_ipv6_address(&addr, node_id);

    // Send the unicast packet
    simple_udp_sendto(&udp_conn, &tmSent, sizeof(tmSent), &addr);
    printf("Node ID: %d, Sending packet to ", node_id);
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