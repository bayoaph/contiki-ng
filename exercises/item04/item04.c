
#include "contiki.h"
//#include "net/rime/rime.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h"
#include "sys/ctimer.h"
#include <stdio.h>

#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h" 


#define UDP_PORT 1234

struct timeMessage {
  clock_time_t time;
  unsigned short originator;
};

static struct timeMessage tmSent, tmReceived;
static struct simple_udp_connection udp_conn;

//void print_temperature_binary_to_float(uint16_t temp) {
  //printf("%d.%d", (temp / 10 - 396) / 10, (temp / 10 - 396) % 10);
//}

//static struct ctimer leds_off_timer_send;

/* Timer callback turns off the blue led */
//static void timerCallback_turnOffLeds() {
  //leds_off(LEDS_RED);
//}

PROCESS(example_udp_process, "RTT using Simple UDP");
AUTOSTART_PROCESSES(&example_udp_process);

static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data, uint16_t datalen) {
  
  // Round-trip time, will be decremented later
  clock_time_t rtt = clock_time();

  //printf("Received packet from ");
  //uip_debug_ipaddr_print(sender_addr);
  //printf("\n");

  //printf("UDP message received\n");
  //leds_on(LEDS_RED);
  //ctimer_set(&leds_off_timer_send, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);
  
  
  memcpy(&tmReceived, data, sizeof(tmReceived));


  printf("Packet received from ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\n");


  // Calculate RTT and print details
  //clock_time_t rtt = current_time - tmReceived.time;
  
  
  printf("time received = %lu clock ticks = %lu secs %lu millis\n",
        (unsigned long)tmReceived.time,
        (unsigned long)tmReceived.time / CLOCK_SECOND,
        (1000L * ((unsigned long)tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND);
  printf("originator = %u\n", tmReceived.originator);


  if (tmReceived.originator != node_id) {
    printf("Sending back the received timestamp\n");
    simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
  } else {
    rtt -= tmReceived.time;
    printf("RTT = %lu ms\n", (1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
  }
}

PROCESS_THREAD(example_udp_process, ev, data) {
  PROCESS_BEGIN();

  //uip_ipaddr_t addr;

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_rx_callback);

  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    tmSent.time = clock_time();
    tmSent.originator = node_id;

    //uip_ip6addr(&addr, 0xfe80, 0, 0, 0, 0, 0, 0, node_id % 2 == 0 ? node_id + 1 : node_id - 1);

    //Store the partner node address permanently in addr
    uip_ipaddr_t addr;
    if (node_id % 2 == 0) {
      uip_create_linklocal_allnodes_mcast(&addr);
    } else {
      uip_create_linklocal_allnodes_mcast(&addr);
    }

    simple_udp_sendto(&udp_conn, &tmSent, sizeof(tmSent), &addr);
    printf("Sending packet to ");
    uip_debug_ipaddr_print(&addr);
    printf("\n");
  }

  SENSORS_DEACTIVATE(button_sensor);
  PROCESS_END();
}



//Given code but rime won't work. Thus change all rime related to simple udp//

/* two timeMessage struct declarations/instantiations */
//static struct timeMessage tmReceived;
//static struct timeMessage tmSent;


//struct timeMessage {
  //clock_time_t time;
  //unsigned short originator;
//};


//void print_temperature_binary_to_float(uint16_t temp) {
  //printf("%d.%d", (temp / 10 - 396) / 10, (temp / 10 - 396) % 10);
//}

//static struct ctimer leds_off_timer_send;

/* Timer callback turns off the blue LED */
//static void timerCallback_turnOffLeds(void *ptr) {
  //leds_off(LEDS_BLUE);
//}

/*---------------------------------------------------------------------------*/
//PROCESS(example_unicast_process, "RTT using Rime Unicast Primitive");
//AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/

//static void recv_uc(struct unicast_conn *c, const linkaddr_t *from);
//static const struct unicast_callbacks unicast_callbacks = {recv_uc};
//static struct unicast_conn uc;

/* specify the address of the unicast */
//static linkaddr_t addr;

/* this function is called when a unicast message is received */
//static void recv_uc(struct unicast_conn *c, const linkaddr_t *from) {
  // Round-trip time, will be decremented later
  //clock_time_t rtt = clock_time();

  //printf("Unicast message received from %d.%d\n", from->u8[0], from->u8[1]);
  
  /* Turn on blue LED */
  //leds_on(LEDS_BLUE);
  /* Set the timer "leds_off_timer" to 1/8 second */
  //ctimer_set(&leds_off_timer_send, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

  /* Read the data from the received packet and store it in tmReceived */
  //packetbuf_copyto(&tmReceived);

  /* Print the contents of the received packet */
  //printf("Time received = %lu clock ticks\n", (unsigned long)tmReceived.time);
  //printf(" = %lu secs \n", (unsigned long)tmReceived.time / CLOCK_SECOND);
  //printf("Originator = %u\n", tmReceived.originator);

  // If the packet received is not ours, send it back to the originator
  //if (tmReceived.originator != node_id) {
    //packetbuf_copyfrom(&tmReceived, sizeof(tmReceived));

    //if (!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      /* Send the unicast message */
      //unicast_send(&uc, &addr);
    //}
    //printf("Sending packet to %u\n", addr.u8[0]);
  //} else { // Our packet has completed a round-trip
    //rtt -= tmReceived.time;
    //printf("RTT = %lu ms\n", (1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
  //}
//}

/*---------------------------------------------------------------------------*/
//PROCESS_THREAD(example_unicast_process, ev, data) {
  //PROCESS_EXITHANDLER(unicast_close(&uc);)

  //PROCESS_BEGIN();

  //unicast_open(&uc, 146, &unicast_callbacks);

  //SENSORS_ACTIVATE(button_sensor);

  // Store the partner node address permanently in addr
  //if (node_id % 2 == 0) {
    //addr.u8[0] = node_id + 1;
  //} else {
    //addr.u8[0] = node_id - 1;
  //}
  //addr.u8[1] = 0;

  //while (1) {
    //PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* When the button is pressed, read the current time and write it to tmSent */
    //tmSent.time = clock_time();
    //tmSent.originator = node_id;

    /* Prepare the unicast packet to be sent */
    //packetbuf_copyfrom(&tmSent, sizeof(tmSent));

    //if (!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
      /* Send the unicast message */
      //unicast_send(&uc, &addr);
    //}
    //printf("Sending packet to %u\n", addr.u8[0]);
  //}

  //SENSORS_DEACTIVATE(button_sensor);

  //PROCESS_END();
//}

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