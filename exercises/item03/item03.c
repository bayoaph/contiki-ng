/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly and Contiki-NG - M. Cabilo, Apr 2019  */

#include "contiki.h"
#include "etimer.h"
#include "simple-udp.h"
#include "dev/leds.h"
#include "node-id.h"
#include "dev/sht25.h"
#include "net/ipv6/uiplib.h"
#include <stdio.h>
#include <string.h>

#define BROADCAST_ADDR "ff02::1"  // IPv6 broadcast address
#define UDP_PORT 1234             // Port for communication

// Struct to hold temperature message data
struct temperature_message {
  char message[50];   // Message to send
  uint16_t temperature;  // Temperature value
};

// UDP connection and timers
static struct simple_udp_connection udp_conn;
static struct ctimer leds_off_timer;  // Timer to turn off LEDs
static struct etimer et;  // Timer for periodic tasks

// Message payload
static struct temperature_message tm;

/*------------------------------------------------------------*/
/* Function to turn off BLUE LEDs (callback for the timer) */
static void turn_off_leds_callback() {
  leds_off(LEDS_YELLOW);
}

/*------------------------------------------------------------*/
/* Function to send a broadcast packet */
static void send_broadcast_packet(struct temperature_message *payload) {
  uip_ipaddr_t broadcast_addr;
  uint8_t buffer[sizeof(struct temperature_message)];

  // Serialize the payload into buffer
  memcpy(buffer, payload, sizeof(struct temperature_message));

  // Convert broadcast address from string to uip_ipaddr_t
  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

  // Send the packet
  simple_udp_sendto(&udp_conn, buffer, sizeof(buffer), &broadcast_addr);
}

/*------------------------------------------------------------*/
/* Callback function to handle received UDP packets */
static void udp_recv_callback(
    struct simple_udp_connection *c,
    const uip_ipaddr_t *sender_addr,
    uint16_t sender_port,
    const uip_ipaddr_t *receiver_addr,
    uint16_t receiver_port,
    const uint8_t *data,
    uint16_t datalen) {

  struct temperature_message received_data;

  // Deserialize the received data into the structure
  memcpy(&received_data, data, sizeof(struct temperature_message));

  // Convert sender address to string for printing
  char sender_addr_str[UIPLIB_IPV6_MAX_STR_LEN];
  uiplib_ipaddr_snprint(sender_addr_str, sizeof(sender_addr_str), sender_addr);

  // Display received message
  printf("Received message from [%s]:%u\n", sender_addr_str, sender_port);
  printf("Message: %s, Temperature: %u\n", received_data.message, received_data.temperature);

  // Blink BLUE LED to indicate message receipt
  leds_on(LEDS_YELLOW);
  ctimer_set(&leds_off_timer, CLOCK_SECOND, turn_off_leds_callback, NULL);
}

/*------------------------------------------------------------*/
/* Process definition for the sender and receiver */
PROCESS(broadcast_rssi_process, "Broadcast RSSI Process");
AUTOSTART_PROCESSES(&broadcast_rssi_process);

PROCESS_THREAD(broadcast_rssi_process, ev, data) {
  PROCESS_BEGIN();

  // Initialize UDP connection and register callback
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

  if (node_id == 1) {
    // Sender node configuration
    printf("Node %u: Starting as sender\n", node_id);
    SENSORS_ACTIVATE(sht25);

    // Set timer to trigger every 4 seconds
    etimer_set(&et, CLOCK_SECOND * 4);

    while (1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));  // Wait for timer event
      etimer_reset(&et);  // Reset the timer

      uint16_t temperature = sht25.value(SHT25_VAL_TEMP); // Read temperature

      if (temperature == (uint16_t)-1) {
        // Handle temperature sensor error
        sprintf(tm.message, "Node %u: Temperature read error", node_id);
        tm.temperature = 0;
      } else {
        // Store temperature value
        sprintf(tm.message, "Node %u: Temperature", node_id);
        tm.temperature = temperature / 10;  // Ensure correct scaling if necessary
      }

      // Send the broadcast message
      send_broadcast_packet(&tm);

      // Blink BLUE LED (blue output ng yellow haha) to indicate message sent
      leds_on(LEDS_YELLOW);
      ctimer_set(&leds_off_timer, CLOCK_SECOND, turn_off_leds_callback, NULL);

      printf("Node %u: Sent broadcast message\n", node_id);
    }

    SENSORS_DEACTIVATE(sht25);  // Deactivate sensor after use

  } else {
    // Receiver node configuration (just listen for messages)
    printf("Node %u: Starting as receiver\n", node_id);
  }

  PROCESS_END();
}

/* Exercise 3a: flash the program to your nodes and observe what happens. Read the code and understand it thoroughly.
 */

/* Exercise 3b (to be solved in ILIAS): for the node with the temperature sensor, read out the temperature value
 * in each loop. Then, send it to the other node using the broadcast function. Use the defined struct "temperatureMessage".
 * Learn how to define and instantiate structs in C on the internet. Replace the payload "helloMessage" entirely with
 * an instance of the struct temperatureMessage defined on line 11
 *
 * struct temperatureMessage {
 *	char messageString[50];
 *	uint16_t temperature;
 * };
 *
 * containing the string (messageString) and the current temperature (temperature).
 * Attention: watch out how you write into the temperatureMessage.messageString field in the C programming language...
 *
 * When received by the other node, let the receiver print the transmitted temperature to the serial port
 * AND let it blink with the blue LED for 1 second!
 *
 * answer:
 * This code implements a communication system using Contiki OS for two nodes: a sender and a receiver. The sender node,
 * equipped with a temperature sensor (SHT25), reads the temperature value periodically and sends a structured message
 * containing both a description and the temperature value via a UDP broadcast. The message is sent every 4 seconds, and
 * the sender blinks a blue LED to indicate that a message has been transmitted. The receiver node listens for incoming
 * broadcasts, prints the received temperature data to the console, and blinks the blue LED for 1 second upon receiving
 * the message. The code uses a simple UDP connection, serializes the message data into a struct (temperature_message),
 * and handles the LED control with a timer to turn off the LED after a brief interval.
 *
 */
