/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly and Contiki-NG - M. Cabilo, Apr 2019  */
/* Further modified to use SHT11 sensor instead of SHT25, following the logic of the given code */

#include "contiki.h"
#include "etimer.h"
#include "simple-udp.h"
#include <stdio.h>     /* For printf() */
#include <string.h>
#include "dev/button-sensor.h"  /* for the button sensor */
#include "dev/sht11-sensor.h"   /* for the SHT11 temperature sensor */
#include "node-id.h"            /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
#include "net/ipv6/uiplib.h"

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // Link-local all nodes multicast address

struct temperatureMessage {
  char messageString[50];
  uint16_t temperature;
};

static struct simple_udp_connection udp_conn;
static struct ctimer leds_off_timer_send;

/* Timer callback turns off the green led */
static void
timerCallback_turnOffLeds(void *ptr)
{
  leds_off(LEDS_GREEN);
}

static void
send_broadcast_packet(struct temperatureMessage *payload)
{
  uip_ipaddr_t broadcast_addr;

  // Create a buffer to hold the serialized data
  uint8_t buffer[sizeof(struct temperatureMessage)];
  memcpy(buffer, payload, sizeof(struct temperatureMessage)); // Serialize

  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
  simple_udp_sendto(&udp_conn, buffer, sizeof(buffer), &broadcast_addr);
}

// Callback function to handle incoming packets
static void
udp_recv_callback(struct simple_udp_connection *c,
                  const uip_ipaddr_t *sender_addr,
                  uint16_t sender_port,
                  const uip_ipaddr_t *receiver_addr,
                  uint16_t receiver_port,
                  const uint8_t *data,
                  uint16_t datalen)
{
  struct temperatureMessage received_data;
  memcpy(&received_data, data, sizeof(struct temperatureMessage)); // Deserialize

  // Create a buffer to hold the sender IP address in string format
  char sender_addr_str[UIPLIB_IPV6_MAX_STR_LEN];
  uiplib_ipaddr_snprint(sender_addr_str, sizeof(sender_addr_str), sender_addr);
  int16_t temperature = received_data.temperature;
  int16_t wholenumber = temperature/100;
  int16_t decimal = temperature %100;
  printf("Received Sensor Data from [%s]:%u: Message = %s, Temperature = %u.%uC\n",
         sender_addr_str, sender_port, received_data.messageString, wholenumber, decimal);

  // Blink green LED for 1 second upon receiving a message
  leds_on(LEDS_GREEN);
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
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
  PROCESS_BEGIN();

  if (node_id == 1) {
    // This mote will be the sender
    SENSORS_ACTIVATE(sht11_sensor);
    printf("Starting broadcast as sender\n");
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

    /* Broadcast every 4 seconds */
    etimer_set(&et, 4 * CLOCK_SECOND);

    while (1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      etimer_reset(&et);

      // Read temperature from sht11 sensor
      int16_t temperature = sht11_sensor.value(SHT11_SENSOR_TEMP);
      if (temperature == -1) {
        printf("Error reading temperature sensor\n");
        snprintf(tm.messageString, sizeof(tm.messageString),
                 "Here is node %u, temperature reading error", node_id);
        tm.temperature = 0;
      } else {
        snprintf(tm.messageString, sizeof(tm.messageString),
                 "Here is node %u, sending the temperature", node_id);
        tm.temperature = (uint16_t)temperature;
      }

      /* send the packet */
      send_broadcast_packet(&tm);

      /* turn on green led */
      leds_on(LEDS_GREEN);
      /* set the timer "leds_off_timer" to 1 second */
      ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
      printf("Sent broadcast message\n");
    }

    SENSORS_DEACTIVATE(sht11_sensor);

  } else {
    // This mote will be the receiver
    printf("Starting as receiver\n");
    simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);
    // No periodic timer set for receiver
    while(1) {
      // Receiver just waits for packets, no timer events needed
      PROCESS_WAIT_EVENT();
    }
  }

  PROCESS_END();
}


/* Exercise 3a: flash the program to your nodes and observe what happens. Read the code and understand it thoroughly.
 */

/* Exercise 3b: for the node with the temperature sensor (node_id == 1),
 * read out the temperature value in each loop. Then, send it to the other node using the broadcast function.
 * Use the defined struct "temperatureMessage".
 *
 * struct temperatureMessage {
 *   char messageString[50];
 *   uint16_t temperature;
 * };
 *
 * containing the string (messageString) and the current temperature (temperature).
 * When received by the other node, let the receiver print the transmitted temperature to the serial port
 * AND let it blink with the green led for 1 second!
 */
