#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h"
#include "sys/rtimer.h"
#include <stdio.h>
#include <string.h>
#include "net/ipv6/uip-debug.h"

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // Link-local all nodes multicast address

struct timeMessage {
  clock_time_t time;
  unsigned short originator;
};

static struct simple_udp_connection udp_conn;

/* two timeMessage struct declaration/instantiations */
static struct timeMessage tmReceived;
static struct timeMessage tmSent;

/* A timer to turn off the LED */
static struct ctimer leds_off_timer_send;

/* Timer callback turns off the yellow LED */
static void timerCallback_turnOffLeds() {
  leds_off(LEDS_YELLOW);
}

PROCESS(example_unicast_process, "RTT using IPv6/UDP");
AUTOSTART_PROCESSES(&example_unicast_process);

/* This function is called whenever a packet is received over our simple UDP connection */
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen) {
  // Record current time for RTT calculation
  clock_time_t rtt = clock_time();

  printf("Message received from: ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\n");

  /* turn on yellow led to indicate reception */
  leds_on(LEDS_YELLOW);
  /* set the timer "leds_off_timer" to 1/8 second */
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

  /* copy the received data into tmReceived */
  memcpy(&tmReceived, data, sizeof(struct timeMessage));

  /* Print the received packet details */
  printf("Time Received = %u clock ticks ", (unsigned int)tmReceived.time);
  printf("= %u secs ", (unsigned int)(tmReceived.time / CLOCK_SECOND));
  printf("%u millis ", (unsigned int)((1000L * (tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND));
  printf(", originator = %d\n", tmReceived.originator);

  // If the packet received is not ours, send it back
  if (tmReceived.originator != node_id) {
    uip_ipaddr_t sender_addr_copy;
    // Typically, you'd send back to the original sender, but here we do a broadcast as in the second code example.
    // If you want to send specifically back to the sender, omit uiplib_ipaddrconv and just use sender_addr_copy = *sender_addr;
    // For now, we mimic the second code by sending back to BROADCAST_ADDR for demonstration.
    uiplib_ipaddrconv(BROADCAST_ADDR, &sender_addr_copy);

    printf("Sending packet back to: ");
    uip_debug_ipaddr_print(&sender_addr_copy);
    printf(", originator = %d\n", tmReceived.originator);

    simple_udp_sendto(&udp_conn, &tmReceived, sizeof(struct timeMessage), &sender_addr_copy);
  } else {
    // We received our own packet back, so calculate RTT
    rtt -= tmReceived.time;
    printf("RTT = %u ms\n", (unsigned int)((1000L * (rtt % CLOCK_SECOND)) / CLOCK_SECOND));
  }
}

PROCESS_THREAD(example_unicast_process, ev, data) {
  PROCESS_BEGIN();

  // Register the UDP connection
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* When the button is pressed, record the current time and our node_id */
    tmSent.time = clock_time();
    tmSent.originator = node_id;

    /* Prepare and send the UDP packet to the broadcast address */
    uip_ipaddr_t broadcast_addr;
    uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

    printf("Sending packet to ");
    uip_debug_ipaddr_print(&broadcast_addr);
    printf("\nTime Sent = %u clock ticks ", (unsigned int)tmSent.time);
    printf("= %u secs ", (unsigned int)(tmSent.time / CLOCK_SECOND));
    printf("%u millis ", (unsigned int)((1000L * (tmSent.time % CLOCK_SECOND)) / CLOCK_SECOND));
    printf(", originator = %d\n", tmSent.originator);

    simple_udp_sendto(&udp_conn, &tmSent, sizeof(struct timeMessage), &broadcast_addr);
  }

  SENSORS_DEACTIVATE(button_sensor);

  PROCESS_END();
}
/*
  *This is ran in SKYMOTE since the button sensor wont work in the COOJAMOTE.
*/
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