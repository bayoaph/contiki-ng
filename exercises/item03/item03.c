/* Based on exercises by Philipp Hurni, University of Bern, December 2013 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "etimer.h"
#include <string.h>
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h" /* get a pointer to the own node id */
#include "dev/zoul-sensors.h"
#include "simple-udp.h"

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // Link-local all nodes multicast address

struct temperatureMessage
{
  char messageString[50];
  uint16_t temperature;
};

static struct simple_udp_connection udp_conn;
static struct ctimer leds_off_timer_send;

/* Timer callback turns off the blue led */
static void timerCallback_turnOffLeds()
{
  leds_off(LEDS_BLUE);
}

static void send_broadcast_packet(struct temperatureMessage *payload)
{
  uip_ipaddr_t broadcast_addr;

  // Create a buffer to hold the serialized data
  uint8_t buffer[sizeof(struct temperatureMessage)];
  memcpy(buffer, payload, sizeof(struct temperatureMessage)); // Serialize

  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
  simple_udp_sendto(&udp_conn, buffer, strlen(buffer), &broadcast_addr);
}

// Callback function to handle incoming packets
static void udp_recv_callback(struct simple_udp_connection *c, const uip_ipaddr_t *sender_addr, const uint8_t *data, uint16_t datalen)
{
  struct temperatureMessage received_data;
  memcpy(&received_data, data, sizeof(struct temperatureMessage)); // Deserialize

  printf("Received Sensor Data: Message = %s, Temperature = %u\n", received_data.messageString, received_data.temperature);
}

/*---------------------------------------------------------------------------*/
/* one timer struct declaration/instantiation */
static struct etimer et;
/* one temperatureMessage struct declaration/instantiation  */
static struct temperatureMessage tm;
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_rssi_process, "Broadcast RSSI");
AUTOSTART_PROCESSES(&broadcast_rssi_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_rssi_process, ev, data)
{
  // PROCESS_EXITHANDLER(broadcast_close(&bc);)
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(cc2538_temp_sensor);

  printf("Starting broadcast\n");
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback)

      /* Broadcast every 4 seconds */
      etimer_set(&et, 4 * CLOCK_SECOND);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);

    // Temperature of 0 means no sensor, checked using node_id an even number
    uint16_t temperature = (node_id % 2 == 0) ? cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED) : 0;

    // Send a different message, depending on the existence of a temperature sensor
    if (node_id % 2 == 0)
      sprintf(tm.messageString, "Here is node %u, sending the temperature", node_id);
    else
      sprintf(tm.messageString, "Here is node %u, I don't have a sensor", node_id);

    // Save the temperature into the struct
    tm.temperature = temperature;

    /* send the packet */
    send_broadcast_packet(&tm);

    /* turn on blue led */
    leds_on(LEDS_BLUE);
    /* set the timer "leds_off_timer" to 1 second */
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
    printf("sent broadcast message\n");
  }

  SENSORS_DEACTIVATE(cc2538_temp_sensor);

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
 *
 */