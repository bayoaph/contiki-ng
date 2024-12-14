/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly and Contiki-NG - M. Cabilo, Apr 2019  */

#include "contiki.h"
#include "etimer.h"
#include <stdio.h>
#include <string.h>
#include "simple-udp.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/light-sensor.h"
#include "node-id.h"
#include "dev/sht25.h"
#include "net/ipv6/uiplib.h"
#include "net/netstack.h"

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // Link-local all-nodes multicast address

struct temperatureMessage {
  char messageString[50];
  uint16_t temperature;
};

static struct simple_udp_connection udp_conn;
static struct ctimer leds_off_timer;

/* Function to turn off the red LED */
static void turn_off_red_led(void *ptr) {
  leds_off(LEDS_RED);
}

/* Send a broadcast packet */
static void send_broadcast_packet(struct temperatureMessage *payload) {
  uip_ipaddr_t broadcast_addr;
  uint8_t buffer[sizeof(struct temperatureMessage)];
  memcpy(buffer, payload, sizeof(struct temperatureMessage)); // Serialize payload

  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
  simple_udp_sendto(&udp_conn, buffer, sizeof(buffer), &broadcast_addr);
}

/* Callback function to handle received packets */
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen) {
  struct temperatureMessage received_data;
  memcpy(&received_data, data, sizeof(struct temperatureMessage)); // Deserialize payload

  char sender_addr_str[UIPLIB_IPV6_MAX_STR_LEN];
  uiplib_ipaddr_snprint(sender_addr_str, sizeof(sender_addr_str), sender_addr);

  printf("Received Sensor Data from [%s]: Message = %s, Temperature = %u\n",
         sender_addr_str, received_data.messageString, received_data.temperature);

  leds_on(LEDS_RED);
  ctimer_set(&leds_off_timer, CLOCK_SECOND, turn_off_red_led, NULL);
}

/* Timer and message struct declarations */
static struct etimer broadcast_timer;
static struct temperatureMessage tm;

/*---------------------------------------------------------------------------*/
PROCESS(broadcast_rssi_process, "Broadcast RSSI");
AUTOSTART_PROCESSES(&broadcast_rssi_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_rssi_process, ev, data) {
  PROCESS_BEGIN();

  /* Activate the temperature sensor */
  SENSORS_ACTIVATE(sht25);

  /* Register the UDP connection with the callback */
  printf("Starting broadcast process\n");
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

  /* Broadcast every 4 seconds */
  etimer_set(&broadcast_timer, 4 * CLOCK_SECOND);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&broadcast_timer));
    etimer_reset(&broadcast_timer);

    /* Get the temperature or set it to 0 if no sensor is available */
    uint16_t temperature = (node_id % 2 == 0) ? sht25.value(SHT25_VAL_TEMP) : 0;

    /* Prepare the message */
    if (node_id % 2 == 0) {
      snprintf(tm.messageString, sizeof(tm.messageString), 
               "Node %u: Sending temperature", node_id);
    } else {
      snprintf(tm.messageString, sizeof(tm.messageString), 
               "Node %u: No temperature sensor", node_id);
    }
    tm.temperature = temperature;

    /* Send the broadcast message */
    send_broadcast_packet(&tm);

    /* Turn on the red LED and set a timer to turn it off */
    leds_on(LEDS_RED);
    ctimer_set(&leds_off_timer, CLOCK_SECOND, turn_off_red_led, NULL);

    printf("Broadcast message sent\n");
  }

  /* Deactivate the temperature sensor before ending the process */
  SENSORS_DEACTIVATE(sht25);

  PROCESS_END();
}

/* Exercise 3a: flash the program to your nodes and observe what happens. Read the code and understand it thoroughly.
 */


/* Exercise 3b (to be solved in ILIAS): for the node with the temperature sensor, read out the temperature value
 * in each loop. Then, send it to the other node using the broadcast function. Use the defined struct "temperatureMessage".
 * Learn how to define and instantiate structs in C on the internet. Replace the payload "helloMessage" entirely with
 * an instance of the struct temperatureMessage defined on line 11
 *
 *
 * containing the string (messageString) and the current temperature (temperature).
 * Attention: watch out how you write into the temperatureMessage.messageString field in the C programming language...
 *
 * When received by the other node, let the receiver print the transmitted temperature to the serial port
 * AND let it blink with the blue LED for 1 second!
 *
 * Each node checks for a temperature sensor. If present, it reads the temperature, creates a 
 * temperatureMessage, and broadcasts it. If not, it sends a message stating the absence of a sensor.
 * Receiving nodes print the message and temperature (if available) to the serial output and blink their
 * red LED for 1 second. Sending nodes blink their red LED to indicate successful transmission, enabling
 * efficient information exchange within the network. 
 */