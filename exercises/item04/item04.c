#include "contiki.h"
#include <stdio.h>
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h"
#include "sys/rtimer.h"

#define UDP_PORT 1234

struct timeMessage {
    clock_time_t time;
    unsigned short originator;
};

// Global variables
static struct timeMessage tmReceived;
static struct timeMessage tmSent;
static struct simple_udp_connection udp_conn;

// Function declarations
static void initialize_udp(void);
static void handle_button_press(void);
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen);
static void send_packet(const uip_ipaddr_t *addr);

/*---------------------------------------------------------------------------*/
PROCESS(example_unicast_process, "RTT using Simple UDP");
AUTOSTART_PROCESSES(&example_unicast_process);
/*---------------------------------------------------------------------------*/

// Initialize UDP and register the callback
static void initialize_udp(void) {
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);
}

// Send packet with current time to the specified address
static void send_packet(const uip_ipaddr_t *addr) {
    tmSent.time = clock_time();
    tmSent.originator = node_id;

    simple_udp_sendto(&udp_conn, &tmSent, sizeof(tmSent), addr);

    printf("Sending packet to ");
    uip_debug_ipaddr_print(addr);
    printf("\n");
}

// Handle button press to send a packet
static void handle_button_press(void) {
    uip_ipaddr_t addr;
    uip_create_linklocal_allnodes_mcast(&addr);
    send_packet(&addr);
}

// Callback function for receiving UDP packets
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen) {
    clock_time_t rtt = clock_time();

    memcpy(&tmReceived, data, sizeof(tmReceived));

    printf("Packet received from ");
    uip_debug_ipaddr_print(sender_addr);
    printf("\n");
    printf("Time received = %lu clock ticks, %lu secs, %lu millis\n",
           (unsigned long)tmReceived.time,
           (unsigned long)tmReceived.time / CLOCK_SECOND,
           (1000L * ((unsigned long)tmReceived.time % CLOCK_SECOND)) / CLOCK_SECOND);
    printf("Originator = %u\n", tmReceived.originator);

    if (tmReceived.originator != node_id) {
        printf("Sending back the received timestamp\n");
        simple_udp_sendto(&udp_conn, &tmReceived, sizeof(tmReceived), sender_addr);
    } else {
        rtt -= tmReceived.time;
        printf("RTT = %lu ms\n",
               (1000L * ((unsigned long)rtt % CLOCK_SECOND)) / CLOCK_SECOND);
    }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_unicast_process, ev, data) {
    PROCESS_BEGIN();

    initialize_udp();
    SENSORS_ACTIVATE(button_sensor);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
        handle_button_press();
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
 * answer:
 * This code implements a simple unicast communication mechanism in Contiki OS to measure round-trip
 * time (RTT) using UDP packets. It initializes a UDP connection on a predefined port and configures
 * a callback function to handle incoming packets. When the user presses a button on a node, the node
 * sends a packet containing its current timestamp and ID to all link-local nodes using multicast. If
 * another node receives the packet, it responds with the received timestamp, enabling the sender to
 * calculate the RTT. The program prints relevant information, such as the timestamp, originator ID, and
 * calculated RTT, to the console. The system demonstrates button-triggered packet transmission and reception
 * using Contiki's sensor and network APIs.
 */