#include "contiki.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "sys/node-id.h"
#include <stdio.h>

#define UDP_PORT 1234

struct timeMessage {
  clock_time_t time;
  unsigned short originator;
};

static struct timeMessage tmReceived;
static struct timeMessage tmSent;
static struct simple_udp_connection udp_conn;

void print_temperature_binary_to_float(uint16_t temp) {
  printf("%d.%d", (temp / 10 - 396) / 10, (temp / 10 - 396) % 10);
}

static struct ctimer leds_off_timer_send;

/* Timer callback turns off the blue LED */
static void timerCallback_turnOffLeds() {
  leds_off(LEDS_GREEN);
}


/* Function to construct an IPv6 address based on node_id */
void construct_ipv6_address(uip_ipaddr_t *addr, uint16_t node_id) {
  linkaddr_t receiver_node_linkaddr;

  // Copy part of the link-layer address to the receiver node's link address
  // these copied bytes are used to retain essential parts of the address (likely the node's identifier portion)
  receiver_node_linkaddr.u8[0] = linkaddr_node_addr.u8[0];
  receiver_node_linkaddr.u8[2] = linkaddr_node_addr.u8[2];
  receiver_node_linkaddr.u8[4] = linkaddr_node_addr.u8[4];

  // Define the byte indices to modify
  int modify_bytes[] = {1, 3, 5, 6, 7};

  // Modify the address based on whether the node_id is even or odd
  for (int i = 0; i < sizeof(modify_bytes)/sizeof(modify_bytes[0]); i++) {
    if (node_id % 2 == 0) {
      // For even node_id, increment some parts and decrement others
      if (i == 0) receiver_node_linkaddr.u8[modify_bytes[i]] = linkaddr_node_addr.u8[modify_bytes[i]] + 1; // Increment the 1st byte
      else receiver_node_linkaddr.u8[modify_bytes[i]] = linkaddr_node_addr.u8[modify_bytes[i]] - 1; // Decrement others
    } else {
      // For odd node_id, decrement some parts and increment others
      if (i == 0) receiver_node_linkaddr.u8[modify_bytes[i]] = linkaddr_node_addr.u8[modify_bytes[i]] - 1; // Decrement the 1st byte
      else receiver_node_linkaddr.u8[modify_bytes[i]] = linkaddr_node_addr.u8[modify_bytes[i]] + 1; // Increment others
    }
  }

  // Combine parts of the link address to form the IPv6 address
  uip_ip6addr(addr, 0xfe80, 0, 0, 0, 0x212,
              (receiver_node_linkaddr.u8[2] << 8) | receiver_node_linkaddr.u8[3], 
              (receiver_node_linkaddr.u8[4] << 8) | receiver_node_linkaddr.u8[5], 
              (receiver_node_linkaddr.u8[6] << 8) | receiver_node_linkaddr.u8[7]); 

  // Print the generated IPv6 address for debugging
  printf("Generated IPv6 address: ");
  uip_debug_ipaddr_print(addr);
  printf("\n");
}



static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data, uint16_t datalen) {
  clock_time_t current_time = clock_time();

  printf("Received packet from ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\n");

  printf("UDP message received\n");
  leds_on(LEDS_GREEN);
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

  if (datalen == sizeof(tmReceived)) {
    memcpy(&tmReceived, data, sizeof(tmReceived));

    printf("Packet received from ");
    uip_debug_ipaddr_print(sender_addr);
    printf("\n");

    printf("time received = %lu clock ticks", (unsigned long)tmReceived.time);
    printf(" = %lu secs ", (unsigned long)tmReceived.time / CLOCK_SECOND);
    printf("%lu millis ", (1000L * ((unsigned long)tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND);
    printf("originator = %u\n", tmReceived.originator);

    if (tmReceived.originator != node_id) {
      printf("Sending back the received timestamp\n");
      simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
    } else {
      clock_time_t rtt = current_time - tmReceived.time;
      printf("RTT = %lu ms\n", (1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
    }
  } else {
    printf("Received unexpected data\n");
  }
}

PROCESS(example_udp_process, "RTT using Simple UDP");
AUTOSTART_PROCESSES(&example_udp_process);

PROCESS_THREAD(example_udp_process, ev, data) {
  PROCESS_BEGIN();

  uip_ipaddr_t addr;

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    tmSent.time = clock_time();
    tmSent.originator = node_id;

    /* Construct IPv6 address */
    construct_ipv6_address(&addr, node_id);

    printf("Node %u attempting to send to ", node_id);
    uip_debug_ipaddr_print(&addr);
    printf("\n");

    simple_udp_sendto(&udp_conn, &tmSent, sizeof(tmSent), &addr);
    printf("Sending packet to peer\n");
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
 * 
 * Answer:
 * The code demonstrates UDP communication between two nodes in Contiki OS using simple udp library. 
 * When a button is pressed, a node sends a packet with its timestamp and originator ID to another node. 
 * The receiving node processes the packet, calculates the round-trip time (RTT) by comparing the current 
 * time with the received timestamp, and sends the same packet back to the sender. This forms a unicast 
 * message exchange, where the sender receives the same timestamp it initially sent. The task is to ensure 
 * the sender sends its timestamp and receives a response with the same timestamp, which is already 
 * implemented in the code. The RTT is printed in milliseconds.
 * 
 */


// Original Code:
// #include "contiki.h"
// #include "net/rime.h"
// #include "dev/button-sensor.h"
// #include "dev/leds.h"
// #include "sys/node-id.h"
// #include "sys/rtimer.h"
// #include <stdio.h>

// struct timeMessage
// {
//   clock_time_t time;
//   unsigned short originator;
// };

// /* two timeMessage struct declaration/instantiations */
// static struct timeMessage tmReceived;
// static struct timeMessage tmSent;

// void print_temperature_binary_to_float(uint16_t temp)
// {
//   printf("%d.%d", (temp / 10 - 396) / 10, (temp / 10 - 396) % 10);
// }

// static struct ctimer leds_off_timer_send;

// /* Timer callback turns off the blue led */
// static void timerCallback_turnOffLeds()
// {
//   leds_off(LEDS_GREEN);
// }

// /*---------------------------------------------------------------------------*/
// PROCESS(example_unicast_process, "RTT using Rime Unicast Primitive");
// AUTOSTART_PROCESSES(&example_unicast_process);
// /*---------------------------------------------------------------------------*/

// static void recv_uc(struct unicast_conn *c, const rimeaddr_t *from);
// static const struct unicast_callbacks unicast_callbacks = {recv_uc};
// static struct unicast_conn uc;

// /* specify the address of the unicast */
// static rimeaddr_t addr;

// /* this function has been defined to be called when a unicast is being received */
// static void recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
// {
//   // Round-trip time, will be decremented later
//   clock_time_t rtt = clock_time();

//   printf("unicast message received from %d.%d\n", from->u8[0], from->u8[1]);
//   /* turn on blue led */
//   leds_on(LEDS_GREEN);
//   /* set the timer "leds_off_timer" to 1/8 second */
//   ctimer_set(&leds_off_timer_send, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

//   /* from the packet we have just received, read the data and write it into the
//    * struct tmReceived we have declared and instantiated above (line 16)
//    */
//   packetbuf_copyto(&tmReceived);

//   /* print the contents of the received packet */
//   printf("time received = %d clock ticks", (uint16_t)tmReceived.time);
//   printf(" = %d secs ", (uint16_t)tmReceived.time / CLOCK_SECOND);
//   printf("%d millis ", (1000L * ((uint16_t)tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND);
//   printf("originator = %d\n", tmReceived.originator);

//   // If the packet received is not ours, send it back to the originator
//   if (tmReceived.originator != node_id)
//   {
//     packetbuf_copyfrom(&tmReceived, sizeof(tmSent));

//     if (!rimeaddr_cmp(&addr, &rimeaddr_node_addr))
//     {
//       /* when calling unicast_send, we have to specify the address as the second argument (a pointer to the defined rimeaddr_t struct) */
//       unicast_send(&uc, &addr);
//     }
//     printf("sending packet to %u\n", addr.u8[0]);
//   }
//   else
//   { // Our packet has completed a round-trip
//     rtt -= tmReceived.time;
//     printf("RTT = %d ms\n", (1000L * ((uint16_t)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
//   }
// }

// /*---------------------------------------------------------------------------*/
// PROCESS_THREAD(example_unicast_process, ev, data)
// {
//   PROCESS_EXITHANDLER(unicast_close(&uc);)

//   PROCESS_BEGIN();

//   unicast_open(&uc, 146, &unicast_callbacks);

//   SENSORS_ACTIVATE(button_sensor);

//   // Store the partner node address permanently in addr.
//   /* in case I am node 50, choose 51 as destination.*/
//   if (node_id % 2 == 0)
//   {
//     addr.u8[0] = node_id + 1;
//   }
//   /* In case I am node 51, choose 50, etc */
//   else
//   {
//     addr.u8[0] = node_id - 1;
//   }
//   addr.u8[1] = 0;

//   while (1)
//   {
//     PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

//     /* when the button is pressed, read the current time and write it to the
//      * previously declared tmSent struct */
//     tmSent.time = clock_time();
//     /* write the id of then node where the button is pressed into the packet */
//     tmSent.originator = node_id;

//     /* prepare the unicast packet to be sent. Write the contents of the struct, where we
//      * have just written the time and the id into, to the packet we intend to send
//      */
//     packetbuf_copyfrom(&tmSent, sizeof(tmSent));

//     if (!rimeaddr_cmp(&addr, &rimeaddr_node_addr))
//     {
//       /* when calling unicast_send, we have to specify the address as the second argument (a pointer to the defined rimeaddr_t struct) */
//       unicast_send(&uc, &addr);
//     }
//     printf("sending packet to %u\n", addr.u8[0]);
//   }

//   SENSORS_DEACTIVATE(button_sensor);

//   PROCESS_END();
// }
