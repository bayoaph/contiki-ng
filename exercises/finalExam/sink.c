#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "dev/leds.h"  // For controlling the LED state
#include "sys/ctimer.h"  // For using timers to control LED off state
#include "net/ipv6/simple-udp.h"  // For simple UDP communication
#include "node-id.h" /* Include this header to access node_id */

#define UDP_PORT 1234 

// Define a structure to hold the temperature message
struct temperatureMessage {
  char messageString[50];  
  uint16_t temperature;  // Raw temperature value (in mV)
};

static struct simple_udp_connection udp_conn;  
static uint8_t led_state = 0;  // Variable to track the current LED state
static struct ctimer leds_off_timer;  

// Function to toggle the LEDs in a cycle (Red -> Yellow -> Green)
void toggle_leds() {
  leds_off(LEDS_ALL);  // Turn off all LEDs
  if (led_state == 0) {
    leds_on(LEDS_RED);  // Turn on the Red LED
  } else if (led_state == 1) {
    leds_on(LEDS_YELLOW);  // Turn on the Yellow LED
  } else {
    leds_on(LEDS_GREEN);  // Turn on the Green LED
  }
  led_state = (led_state + 1) % 3;  // Cycle through LED states 
}

// Callback function for the timer to turn off the LEDs after a delay
void timer_callback(void *ptr) {
  leds_off(LEDS_ALL);  
}

// UDP receive callback function, triggered when a UDP packet is received
static void udp_recv_callback(struct simple_udp_connection *c,
                               const uip_ipaddr_t *sender_addr,
                               uint16_t sender_port,
                               const uip_ipaddr_t *receiver_addr,
                               uint16_t receiver_port,
                               const uint8_t *data,
                               uint16_t datalen) {
  // Structure to hold received data
  struct temperatureMessage received_data;  
  // Copy the received data into the structure
  memcpy(&received_data, data, sizeof(struct temperatureMessage));  
  // Extract the raw temperature value (in mV)
  uint16_t temperature = received_data.temperature;  

  // Convert the temperature from mV to Celsius (based on sensor's specifications)
  uint16_t readable_temp = (temperature - 270) / 10;  // Adjust the 270 offset as needed for your sensor

  // Print the received message and the readable temperature (after conversion)
  printf("Received by sink node %u: %s, Temperature: %u.%uC\nTemperature (without offset): %uC\n", 
         node_id, received_data.messageString, temperature / 10, temperature % 10, readable_temp);
  
  toggle_leds();  
  ctimer_set(&leds_off_timer, CLOCK_SECOND, timer_callback, NULL);  // Set a timer to turn off LEDs after 1 second
}

// Main process for the sink node
PROCESS(sink_node_process, "Sink Node Process");
AUTOSTART_PROCESSES(&sink_node_process); 

PROCESS_THREAD(sink_node_process, ev, data) {
  PROCESS_BEGIN(); 

  printf("Sink node started\n");  
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);  // Register the UDP connection for receiving data

  while (1) {
    PROCESS_WAIT_EVENT();  
  }

  PROCESS_END();  
}
