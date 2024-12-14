/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly and Contiki-NG - M. Cabilo, Apr 2019  */

#include "contiki.h"
#include "etimer.h"
#include <stdio.h>
#include <string.h>
#include "dev/leds.h"
#include "dev/sensor/sht11/sht11-sensor.h"
#include "node-id.h"
#include "simple-udp.h"
#include "net/ipv6/uiplib.h"

/*---------------------------------------------------------------------------*/
/* Define message struct */
struct temperatureMessage {
  char messageString[50];
  uint16_t temperature;
};

/*---------------------------------------------------------------------------*/
/* Definitions */
#define UDP_PORT 1234               // Port for UDP communication
#define BROADCAST_ADDR "ff02::1"    // IPv6 broadcast address
#define SEND_INTERVAL (4 * CLOCK_SECOND)  // 4-second broadcast interval

static struct simple_udp_connection udp_conn;
static struct ctimer leds_off_timer;   // Timer to turn off LEDs
static struct etimer et;              // Event timer
static struct temperatureMessage tm;  // Message structure

/*---------------------------------------------------------------------------*/
/* Function Prototypes */
static void turn_off_leds_callback();
static void send_broadcast_packet(const struct temperatureMessage *payload);

/*---------------------------------------------------------------------------*/
/* LED callback: turns off yellow LED */
static void turn_off_leds_callback() {
  leds_off(LEDS_YELLOW);
}

/*---------------------------------------------------------------------------*/
/* Send a broadcast UDP message */
static void send_broadcast_packet(const struct temperatureMessage *payload) {
  uip_ipaddr_t broadcast_addr;
  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
  simple_udp_sendto(&udp_conn, payload, sizeof(*payload), &broadcast_addr);
}

/*---------------------------------------------------------------------------*/
/* UDP receive callback */
static void udp_recv_callback(struct simple_udp_connection *c,
                               const uip_ipaddr_t *sender_addr,
                               uint16_t sender_port,
                               const uip_ipaddr_t *receiver_addr,
                               uint16_t receiver_port,
                               const uint8_t *data,
                               uint16_t datalen) {
  struct temperatureMessage received_data;
  memcpy(&received_data, data, sizeof(received_data));

  char sender_ip[UIPLIB_IPV6_MAX_STR_LEN];
  uiplib_ipaddr_snprint(sender_ip, sizeof(sender_ip), sender_addr);

  printf("Received message from [%s]: %u\n", sender_ip, sender_port);
  printf("Message: %s, Temperature: %u\n", received_data.messageString, received_data.temperature);

  /* Blink yellow LED for 1 second */
  leds_on(LEDS_YELLOW);
  ctimer_set(&leds_off_timer, CLOCK_SECOND, turn_off_leds_callback, NULL);
}

/*---------------------------------------------------------------------------*/
/* Main process */
PROCESS(broadcast_rssi_process, "Broadcast RSSI Process");
AUTOSTART_PROCESSES(&broadcast_rssi_process);

PROCESS_THREAD(broadcast_rssi_process, ev, data) {
  PROCESS_BEGIN();

  /* Register UDP connection */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

  /* Sender node logic */
  if (node_id == 1) {
    printf("Node %u: Starting as sender\n", node_id);
    SENSORS_ACTIVATE(sht11_sensor);  // Activate temperature sensor
    etimer_set(&et, SEND_INTERVAL);

    while (1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      etimer_reset(&et);

      /* Read temperature */
      uint16_t temperature = sht11_sensor.value(SHT11_SENSOR_TEMP);
      if (temperature == (uint16_t)-1) {
        sprintf(tm.messageString, "Node %u: Temperature error", node_id);
        tm.temperature = 0;
      } else {
        sprintf(tm.messageString, "Node %u: Temperature", node_id);
        tm.temperature = temperature / 10;  // Convert to suitable format
      }

      /* Send broadcast */
      send_broadcast_packet(&tm);

      /* Indicate sending with yellow LED */
      leds_on(LEDS_YELLOW);
      ctimer_set(&leds_off_timer, CLOCK_SECOND, turn_off_leds_callback, NULL);

      printf("Node %u: Sent broadcast message\n", node_id);
    }

    SENSORS_DEACTIVATE(sht11_sensor);  // Deactivate sensor

  } else {
    /* Receiver node logic */
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
 *
 // My answer: 
 // Sender node -> Activates the temperature sensor.
 -> Reads temperature every 4 seconds.
 -> Packages the data in a message structure.
 -> Sends it as a broadcast UDP message.
 -> Blinks yellow LED to indicate message sent.
 
 // Receiver node -> Listens for incoming UDP messages.
 -> Receives and extracts the temperature data.
 -> Prints the message and temperature.
 -> Blinks yellow LED to indicate message received.

 // Code Modifications  
-> Replaced basic string payload with a structured message (`temperatureMessage`).  
-> Integrated `simple_udp` API for sending and receiving UDP packets.  
-> Added functionality to read temperature data from the SHT11 sensor.  
-> Configured sender node to broadcast temperature readings every 4 seconds.  
-> Enabled receiver node to extract and print temperature data from incoming messages.  
-> Implemented LED feedback: yellow LED blinks on both send and receive events.  

 */