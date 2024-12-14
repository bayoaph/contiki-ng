
#include "contiki.h"
//#include "net/rime.h"
#include "net/ipv6/simple-udp.h" //used instead of rime
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h"
#include "sys/rtimer.h"
#include <stdio.h>



struct timeMessage
{
  clock_time_t time;
  unsigned short originator;
};

/* two timeMessage struct declaration/instantiations */
static struct timeMessage tmReceived;
static struct timeMessage tmSent;

void print_temperature_binary_to_float(uint16_t temp)
{
  printf("%d.%d", (temp / 10 - 396) / 10, (temp / 10 - 396) % 10);
}

static struct ctimer leds_off_timer_send;

/* Timer callback turns off the blue led */
static void timerCallback_turnOffLeds()
{
  //leds_off(LEDS_BLUE); 
  leds_off(LEDS_GREEN); //no blue led in cooja
}

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "RTT using Simple UDP");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/

/* Corrected callback function signature */
static void udp_callback(struct simple_udp_connection *c,
                         const uip_ipaddr_t *sender_addr,
                         uint16_t sender_port,
                         const uip_ipaddr_t *receiver_addr,
                         uint16_t receiver_port,
                         const uint8_t *data,
                         uint16_t datalen);
static struct simple_udp_connection udp_conn;

/* specify the address of the UDP destination */
static uip_ipaddr_t addr;

/* this function has been defined to be called when a UDP packet is received */
static void udp_callback(struct simple_udp_connection *c,
                         const uip_ipaddr_t *sender_addr,
                         uint16_t sender_port,
                         const uip_ipaddr_t *receiver_addr,
                         uint16_t receiver_port,
                         const uint8_t *data,
                         uint16_t datalen)
{
    // Round-trip time, will be decremented later
    clock_time_t rtt = clock_time();

    printf("UDP message received from %u.%u\n", sender_addr->u8[0], sender_addr->u8[1]);

    /* turn on blue led */
    leds_on(LEDS_GREEN);
    /* set the timer "leds_off_timer" to 1/8 second */
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND / 8, timerCallback_turnOffLeds, NULL);

    /* from the packet we have just received, read the data and write it into the
   * struct tmReceived we have declared and instantiated above (line 16)
   */
    memcpy(&tmReceived, data, sizeof(tmReceived));

    /* print the contents of the received packet */
    printf("time received = %lu clock ticks", (unsigned long)tmReceived.time);
    printf(" = %lu secs ", (unsigned long)tmReceived.time / CLOCK_SECOND);
    printf("%lu millis ", (1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
    printf("originator = %d\n", tmReceived.originator);

    // If the packet received is not ours, send it back to the originator
    if (tmReceived.originator != node_id)
    {  
        simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
        printf("sending packet to %u\n", tmReceived.originator);
    }

    else
    { // Our packet has completed a round-trip
        rtt -= tmReceived.time;
        printf("RTT = %lu ms\n", (unsigned long)(1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
    }
}



void get_destination_address() {
    linkaddr_t dest_addr = linkaddr_node_addr;
    
   // Determine destination node ID based on whether the current node ID is even or odd
  if (node_id % 2 == 0) {
    dest_addr.u8[1] += 1; 
    dest_addr.u8[3] -= 1;
    dest_addr.u8[5] -= 1;
    dest_addr.u8[6] -= 1;
    dest_addr.u8[7] -= 1;
  } else {
    dest_addr.u8[1] -= 1; 
    dest_addr.u8[3] += 1;
    dest_addr.u8[5] += 1;
    dest_addr.u8[6] += 1;
    dest_addr.u8[7] += 1;
  }

    // Construct the destination IPv6 address dynamically
        uip_ip6addr(&addr, 0xfe80, 0x0000, 0x0000,0x0000, 0x212,
              (dest_addr.u8[2] << 8) | dest_addr.u8[3],
              (dest_addr.u8[4] << 8) | dest_addr.u8[5],
              (dest_addr.u8[6] << 8) | dest_addr.u8[7]);   
}

/* Function to print an IPv6 address */
void print_ipv6_address(uip_ipaddr_t *ip_addr) {
  printf("Sending packet to IP address: ");
  for (int i = 0; i < 8; i++) {
    printf("%x", ip_addr->u8[i*2] << 8 | ip_addr->u8[i*2 + 1]);
    if (i < 7) {
      printf(":");
    }
  }
  printf("\n");
}


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data)
{
  PROCESS_EXITHANDLER(memset(&udp_conn, 0, sizeof(udp_conn));)

  PROCESS_BEGIN();

  printf("Node ID: %u, Destination ID: %u\n", node_id, addr.u8[0]);


  /* open the UDP connection with the correct type and callback */
  simple_udp_register(&udp_conn, 1234, NULL, 1234, udp_callback);


  SENSORS_ACTIVATE(button_sensor);


  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* when the button is pressed, read the current time and write it to the
     * previously declared tmSent struct */
    tmSent.time = clock_time();
    /* write the id of the node where the button is pressed into the packet */
    tmSent.originator = node_id;

    // Store the partner node address permanently in addr.
    /* in case I am node 50, choose 51 as destination. */
    memset(&addr, 0, sizeof(addr));

    get_destination_address();



    /* send the packet */
    print_ipv6_address(&addr);

    simple_udp_sendto(&udp_conn, &tmSent, sizeof(tmSent), &addr);

    
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
 * ANSWER:
 * I have used the UDP library for unicasting instead of the Rime. Each node in the  simulation can send and receive messages over the network.
 * When a button is pressed on the node, the current time is recorded and included in a message sent to the next node (if even sent to the next node, if odd sent to the previous node).
 * The receiver turns on the green LED, and if the packet was from a different node, it is sent back to the sender.
    while if it gotton back by the sender, its round trip is calculated and displayed. The hard part of this exercise was changing the rime functions to the udp.
    Especially with the addressing where i need to find its 8 byte address of the destination. 

    To send the packet back to its originator it resent with the same function in the callback function.

    if (tmReceived.originator != node_id)
    {  
        simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
        printf("sending packet to %u\n", tmReceived.originator);
    }

 * 
 */