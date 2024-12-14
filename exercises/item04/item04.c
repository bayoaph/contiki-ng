
#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h"
#include "sys/rtimer.h"
#include <stdio.h>
#include "net/ipv6/uip-debug.h"

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1"


struct timeMessage
{
  clock_time_t time;
  unsigned short originator;
};


static struct simple_udp_connection udp_conn;

/* two timeMessage struct declaration/instantiations */
static struct timeMessage tmReceived;
static struct timeMessage tmSent;

// void print_temperature_binary_to_float(uint16_t temp)
// {
//   printf("%d.%d", (temp / 10 - 396) / 10, (temp / 10 - 396) % 10);
// }

static struct ctimer leds_off_timer_send;

/* Timer callback turns off the blue led */
static void timerCallback_turnOffLeds()
{
  leds_off(LEDS_YELLOW);
}

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "RTT using Rime Unicast Primitive");
AUTOSTART_PROCESSES(&example_unicast_process);

// static void recv_uc(struct unicast_conn *c, const rimeaddr_t *from);
// static const struct unicast_callbacks unicast_callbacks = {recv_uc};
// static struct unicast_conn uc;

// /* specify the address of the unicast */
// static rimeaddr_t addr;

/* this function has been defined to be called when a unicast is being received */
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen){
  // Round-trip time, will be decremented later
  clock_time_t rtt = clock_time();

  printf("unicast message received from: ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\n");

  /* turn on blue led */
  leds_on(LEDS_YELLOW);

  /* set the timer "leds_off_timer" to 1/8 second */
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

  /* from the packet we have just received, read the data and write it into the
   * struct tmReceived we have declared and instantiated above (line 16)
   */
  //packetbuf_copyto(&tmReceived);
  
  memcpy(&tmReceived,data,sizeof(struct timeMessage));//copt recieved data to tmRecieved
  /* print the contents of the received packet */
  printf("time received = %d clock ticks", (uint16_t)tmReceived.time);
  printf(" = %lu secs ", (uint16_t)tmReceived.time / CLOCK_SECOND);
  printf("%lu millis ", (1000L * ((uint16_t)tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND);
  printf(" originator = %d\n", tmReceived.originator);

  // If the packet received is not ours, send it back to the originator
  if (tmReceived.originator != node_id)
  {
    // packetbuf_copyfrom(&tmReceived, sizeof(tmSent));

    // if (!rimeaddr_cmp(&addr, &rimeaddr_node_addr))
    // {
    //   /* when calling unicast_send, we have to specify the address as the second argument (a pointer to the defined rimeaddr_t struct) */
    //   unicast_send(&uc, &addr);
    // }
   
    simple_udp_sendto(&udp_conn, &tmReceived, sizeof(struct timeMessage), sender_addr);

    printf("sending packet back to:");
    uip_debug_ipaddr_print(sender_addr);
    printf("originator = %d\n",tmReceived.originator);
  }
  else
  { // Our packet has completed a round-trip
    rtt -= tmReceived.time;
     printf("RTT = %lu ms\n", (1000L * ((uint16_t)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  //PROCESS_EXITHANDLER(unicast_close(&uc);)

  PROCESS_BEGIN();

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

  SENSORS_ACTIVATE(button_sensor);

  // Store the partner node address permanently in addr.
  /* in case I am node 50, choose 51 as destination.*/
  // if (node_id % 2 == 0)
  // {
  //   addr.u8[0] = node_id + 1;
  // }
  // /* In case I am node 51, choose 50, etc */
  // else
  // {
  //   addr.u8[0] = node_id - 1;
  // }
  // addr.u8[1] = 0;

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
    // packetbuf_copyfrom(&tmSent, sizeof(tmSent));
    
    uip_ipaddr_t broadcast_addr;
    uip_create_linklocal_allnodes_mcast(&broadcast_addr);

    printf("Sending packet to ");
    uip_debug_ipaddr_print(&broadcast_addr);
    printf("\n");

    simple_udp_sendto(&udp_conn, &tmSent, sizeof(struct timeMessage), &broadcast_addr);
    // if (!rimeaddr_cmp(&addr, &rimeaddr_node_addr))
    // {
    //   /* when calling unicast_send, we have to specify the address as the second argument (a pointer to the defined rimeaddr_t struct) */
    //   unicast_send(&uc, &addr);
    // }
    //printf("sending packet to %u\n", addr.u8[0]);
   
  }

  SENSORS_DEACTIVATE(button_sensor);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
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
 /*
 When the button is pressed on a node, it sends its current time to another node, which then sends it back.
 The sender calculates how long it took for the message to travel back and forth, showing the round-trip time (RTT).
 */