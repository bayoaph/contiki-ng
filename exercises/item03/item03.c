/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly and Contiki-NG - M. Cabilo, Apr 2019  */

#include "contiki.h"
#include <stdio.h>
#include "sys/etimer.h"
#include <string.h>
#include "dev/leds.h"
#include "sys/node-id.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include <stdlib.h>

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1"
struct temperatureMessage
{
  char messageString[50];
  uint16_t temperature;
};

#define UDP_PORT 1234
#define SEND_INTERVAL (10 * CLOCK_SECOND) // Interval to send broadcast message

/* Timer callback to turn off the blue LED */
static void timerCallback_turnOffLeds()
{
  leds_off(LEDS_GREEN);
  leds_off(LEDS_YELLOW);
  leds_off(LEDS_BLUE);
}

static void send_broadcast_packet(struct temperatureMessage *payload)
{
  uip_ipaddr_t broadcast_addr;

  // Create a buffer to hold the serialized data
  uint8_t buffer[sizeof(struct temperatureMessage)];
  memcpy(buffer, payload, sizeof(struct temperatureMessage)); // Serialize the data

  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
  simple_udp_sendto(&udp_conn, buffer, sizeof(struct temperatureMessage), &broadcast_addr);
}

// Callback function to handle incoming packets
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen)
{
  struct temperatureMessage received_data;
  memcpy(&received_data, data, sizeof(struct temperatureMessage)); // Deserialize the received data

  leds_on(LEDS_YELLOW);
  /* Set the timer "leds_off_timer" to turn off the LED after 1 second */
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
  
  // Print the received message and temperature to the serial port
  printf("Received Sensor Data: Message = %s, Temperature = %u\n", received_data.messageString, received_data.temperature);

  // Blink the blue LED for 1 second to indicate reception
  leds_on(LEDS_BLUE);
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
}

/*---------------------------------------------------------------------------*/
/* One timer struct declaration/instantiation */
static struct etimer et;
/* One temperatureMessage struct declaration/instantiation */
static struct temperatureMessage tm;
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_rssi_process, "Broadcast RSSI");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_rssi_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Starting broadcast\n");
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);
  /* Broadcast every 4 seconds */
  etimer_set(&et, 4 * CLOCK_SECOND);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);

    // Generate random temperature for nodes with a sensor (even node_id)
    uint16_t temperature = (node_id % 2 == 0) ? (rand() % 40000) : 0;

    // Send a different message, depending on the existence of a temperature sensor
    if (node_id % 2 == 0)
      sprintf(tm.messageString, "Node %u sending temperature data", node_id);
    else
      sprintf(tm.messageString, "Node %u has no sensor", node_id);

    // Save the temperature into the struct
    tm.temperature = temperature;

    /* Send the packet */
    send_broadcast_packet(&tm);
    
    /* Turn on the green LED to indicate a message was sent */
    leds_on(LEDS_GREEN);
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
    
    printf("Sent broadcast message: %s, Temperature = %u\n", tm.messageString, tm.temperature);
  }

  PROCESS_END();
}
/* Exercise 3a: Flash the program to your nodes and observe the behavior. 
 * The nodes with a temperature sensor should send their temperature. 
 * Nodes without a sensor send a message indicating this. 
 * Received messages are printed to the serial interface, and the yellow LED blinks.
 */
/*
Nodes with an even node_id send temperature data, while nodes with an odd node_id say they don’t have a sensor.
The green LED lights up briefly when a message is sent, and the serial output shows the node's status.
*/


/* Exercise 3b: For the node with the temperature sensor, read out the temperature value 
 * in each loop. Then, send it to the other node using the broadcast function. 
 * Replace the payload "helloMessage" entirely with the instance of "temperatureMessage" defined 
 * at the beginning of this program.
 *
 * The receiving node should print the temperature to the serial port and blink the blue LED for 1 second.
 */
 /*
 changes to apply:
 *inside the broadcast_rssi_process
uint16_t temperature = (node_id % 2 == 0) ? (rand() % 40000) : 0;

if (node_id % 2 == 0)
  sprintf(tm.messageString, "Node %u sending temperature data", node_id);
else
  sprintf(tm.messageString, "Node %u has no sensor", node_id);

tm.temperature = temperature;
send_broadcast_packet(&tm);

leds_on(LEDS_GREEN);
ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);

printf("Sent broadcast message\n");

  *in the udp_recv_callback
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen)
{
  struct temperatureMessage received_data;
  memcpy(&received_data, data, sizeof(struct temperatureMessage)); // Deserialize the received data

  leds_on(LEDS_BLUE);
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
  printf("Received Sensor Data: Message = %s, Temperature = %u\n", received_data.messageString, received_data.temperature);
}

Nodes with sensors send random temperature values, and the receiving node blinks its
blue LED for 1 second and prints the temperature to confirm it received the data correctly.
 */
