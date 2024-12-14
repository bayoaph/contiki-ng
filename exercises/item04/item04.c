#include "contiki.h"
#include <stdio.h>
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-debug.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h"
#include "sys/clock.h"   // Clock functionality
#include "lib/random.h"  // Random delay (optional)

#define UDP_PORT 1234
#define SEND_INTERVAL (5 * CLOCK_SECOND)  // Interval to send packets if needed

// Structure to hold time and node id
struct TimeMessage {
    clock_time_t timestamp;       // Timestamp
    unsigned short originator_id;  // Node ID
};

// Global variables
static struct TimeMessage received_message;
static struct TimeMessage sent_message;
static struct simple_udp_connection udp_connection;

// Function declarations
static void initialize_udp(void);
static void handle_button_press(void);
static void udp_recv_callback(struct simple_udp_connection *connection,
                              const uip_ipaddr_t *sender_address,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_address,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen);
static void send_packet(const uip_ipaddr_t *address);
static void turn_off_leds(void);

/*---------------------------------------------------------------------------*/
PROCESS(example_rtt_process, "RTT with Simple UDP");
AUTOSTART_PROCESSES(&example_rtt_process);
/*---------------------------------------------------------------------------*/

// Initialize UDP connection and register callback
static void initialize_udp(void) {
    simple_udp_register(&udp_connection, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);
    printf("UDP initialized on port %d\n", UDP_PORT);
}

// Send packet to specified IP address
static void send_packet(const uip_ipaddr_t *address) {
    sent_message.timestamp = clock_time();
    sent_message.originator_id = node_id;

    printf("Sending packet to ");
    uip_debug_ipaddr_print(address);
    printf(" | Time: %lu ticks\n", (unsigned long)sent_message.timestamp);

    simple_udp_sendto(&udp_connection, &sent_message, sizeof(sent_message), address);
}

// Turn off all LEDs
static void turn_off_leds(void) {
    leds_off(LEDS_ALL);
}

// Handle button press (user input)
static void handle_button_press(void) {
    uip_ipaddr_t address;

    // Send to all nodes (multicast)
    uip_create_linklocal_allnodes_mcast(&address);
    send_packet(&address);

    // Turn on blue LED to indicate packet sending
    leds_on(LEDS_BLUE);
}

// Callback function for receiving UDP packets
static void udp_recv_callback(struct simple_udp_connection *connection,
                              const uip_ipaddr_t *sender_address,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_address,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen) {
    clock_time_t round_trip_time = clock_time();  // Get current time
    memcpy(&received_message, data, sizeof(received_message));

    printf("Packet received from ");
    uip_debug_ipaddr_print(sender_address);
    printf("\nTime received = %lu clock ticks\n", (unsigned long)received_message.timestamp);
    printf("Originator = %u\n", received_message.originator_id);

    if (received_message.originator_id != node_id) {
        printf("Sending back timestamp to sender\n");
        simple_udp_sendto(&udp_connection, &received_message, sizeof(received_message), sender_address);
    } else {
        round_trip_time -= received_message.timestamp;  // Calculate RTT
        printf("RTT = %lu ms\n",
               (1000L * ((unsigned long)round_trip_time % CLOCK_SECOND)) / CLOCK_SECOND);

        // Visual indication of RTT result
        leds_toggle(LEDS_RED);
        clock_wait(CLOCK_SECOND / 4);  // Small delay for visibility
        turn_off_leds();  // Turn off LEDs after RTT calculation
    }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_rtt_process, ev, data) {
    PROCESS_BEGIN();

    initialize_udp();
    SENSORS_ACTIVATE(button_sensor);

    printf("RTT measurement application started\n");

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
        handle_button_press();
    }

    SENSORS_DEACTIVATE(button_sensor);

    PROCESS_END();
}


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


 My answer: 

 // Sender node  
-> Activates the button sensor.  
-> On button press, packages the current time and node ID into a message.  
-> Sends the message to all nodes using multicast UDP.  
-> Prints the sent message details.  

// Receiver node  
-> Listens for incoming UDP messages.  
-> If the packet is from another node, sends the message back to the sender.  
-> If the packet is from itself, calculates RTT and prints the result.  
-> Toggles the red LED to indicate RTT calculation.  

// Code Modifications  
-> Replaced Rime unicast with UDP for communication.  
-> Used `simple_udp` API to simplify packet handling.  
-> Added multicast support to send packets to all nodes.  
-> Updated logic to send the same timestamp back to the sender.  
-> Calculated RTT upon receiving the echoed packet.  
-> Added LED toggling for visual feedback on RTT completion.
 */