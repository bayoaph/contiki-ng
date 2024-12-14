#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "net/linkaddr.h"
#include "net/ipv6/uip-debug.h"
#include "node-id.h"
#include <stdio.h>


#define UDP_PORT 1234


/* Define message structure using typedef for clarity */
typedef struct {
  clock_time_t timestamp; /* Time of message creation */
  unsigned short sender_id; /* ID of the node sending the message */
} time_message_t;


/* Declare UDP connection and LED off timer */
static struct simple_udp_connection udp_conn;
static struct ctimer leds_off_timer;


/* Declare variables for received and sent time messages */
static time_message_t tmReceived;
static time_message_t tmSent;


/* Function to turn off the green LED after a short time */
static void turn_off_leds_callback() {
  leds_off(LEDS_GREEN);
}


/* Function to calculate the neighbor's IPv6 address based on node ID */
static void generate_ipv6_address_for_neighbor(uip_ipaddr_t *addr, uint16_t node_id) {
  linkaddr_t target_linkaddr;


  /* Modify link address depending on whether node ID is odd or even */
  if (node_id % 2 == 0) {
    target_linkaddr.u8[0] = linkaddr_node_addr.u8[0];
    target_linkaddr.u8[1] = linkaddr_node_addr.u8[1] + 1; 
    target_linkaddr.u8[2] = linkaddr_node_addr.u8[2];
    target_linkaddr.u8[3] = linkaddr_node_addr.u8[3] - 1;
    target_linkaddr.u8[4] = linkaddr_node_addr.u8[4];
    target_linkaddr.u8[5] = linkaddr_node_addr.u8[5] - 1;
    target_linkaddr.u8[6] = linkaddr_node_addr.u8[6] - 1;
    target_linkaddr.u8[7] = linkaddr_node_addr.u8[7] - 1;
  } else {
    target_linkaddr.u8[0] = linkaddr_node_addr.u8[0];
    target_linkaddr.u8[1] = linkaddr_node_addr.u8[1] - 1;
    target_linkaddr.u8[2] = linkaddr_node_addr.u8[2];
    target_linkaddr.u8[3] = linkaddr_node_addr.u8[3] + 1;
    target_linkaddr.u8[4] = linkaddr_node_addr.u8[4];
    target_linkaddr.u8[5] = linkaddr_node_addr.u8[5] + 1;
    target_linkaddr.u8[6] = linkaddr_node_addr.u8[6] + 1;
    target_linkaddr.u8[7] = linkaddr_node_addr.u8[7] + 1;
  }


  /* Create IPv6 address using modified link address */
  uip_ip6addr(addr, 0xfe80, 0, 0, 0, 0x212,
              (target_linkaddr.u8[2] << 8) | target_linkaddr.u8[3],
              (target_linkaddr.u8[4] << 8) | target_linkaddr.u8[5],
              (target_linkaddr.u8[6] << 8) | target_linkaddr.u8[7]);
}


/* Callback function to process received UDP packets */
static void process_received_udp_packet(struct simple_udp_connection *conn,
                                         const uip_ipaddr_t *sender_addr,
                                         uint16_t sender_port,
                                         const uip_ipaddr_t *receiver_addr,
                                         uint16_t receiver_port,
                                         const uint8_t *data,
                                         uint16_t datalen) {
  clock_time_t rtt = clock_time(); /* Round-trip time calculation */


  printf("Received message from ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\n");


  /* Turn on the green LED when message is received */
  leds_on(LEDS_GREEN);


  /* Set timer to turn off the green LED after a brief period */
  ctimer_set(&leds_off_timer, CLOCK_SECOND / 8, turn_off_leds_callback, NULL);


  /* Copy received data into the tmReceived variable */
  memcpy(&tmReceived, data, sizeof(tmReceived));


  /* Print the details of the received message */
  printf("Time received = %lu clock ticks", (unsigned long)tmReceived.timestamp);
  printf(" = %lu secs ", (unsigned long)tmReceived.timestamp / CLOCK_SECOND);
  printf("%lu millis ", (1000L * ((unsigned long)tmReceived.timestamp % CLOCK_SECOND)) / CLOCK_SECOND);
  printf("Sender ID = %d\n", tmReceived.sender_id);


  if (tmReceived.sender_id != node_id) {
    /* Echo the message back to the sender */
    simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
    printf("Echoing message back to sender.\n");
  } else {
    /* Calculate round-trip time (RTT) */
    rtt -= tmReceived.timestamp;
    printf("Round-trip time (RTT) = %lu ms\n", (1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
  }
}


/* Define the main process to handle unicast messages and time tracking */
PROCESS(unicast_rtt_process, "Unicast RTT Process using Simple UDP");
AUTOSTART_PROCESSES(&unicast_rtt_process);


PROCESS_THREAD(unicast_rtt_process, ev, data) {
  static uip_ipaddr_t neighbor_ipaddr;


  PROCESS_BEGIN();


  /* Register UDP connection for communication */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, process_received_udp_packet);


  /* Activate the button sensor */
  SENSORS_ACTIVATE(button_sensor);


  while (1) {
    /* Wait for the button press event */
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);


    /* Prepare the message with the current time and sender's node ID */
    tmSent.timestamp = clock_time();
    tmSent.sender_id = node_id;


    /* Generate the neighbor's IPv6 address based on the node ID */
    generate_ipv6_address_for_neighbor(&neighbor_ipaddr, node_id);


    /* Send the message as a unicast UDP packet */
    simple_udp_sendto(&udp_conn, &tmSent, sizeof(tmSent), &neighbor_ipaddr);
    printf("Node ID: %d, Sending message to ", node_id);
    uip_debug_ipaddr_print(&neighbor_ipaddr);
    printf("\n");
  }


  /* Deactivate the button sensor after use */
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
 * 
 * //Observations
	First, I modified the original code and replaced rime because it is not available in the source and use simple udp instead to get the expected output. 
  From the cooja, I set 4 nodes and the nodes send and receive from each other once the button is clicked and the leds shows the path of the transmission clearly.

 */

