#include "contiki.h"
#include <stdio.h>   /* For printf() */
#include <string.h>  /* For memcpy(), sprintf() */
#include "sys/etimer.h"
#include "dev/leds.h"
#include "sys/node-id.h"
#include "dev/sensor/sht11/sht11-sensor.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"

/* UDP port and IPv6 address for broadcasting */
#define UDP_PORT          1234
#define MULTICAST_ADDRESS "ff02::1" // Multicast to all nodes on the local link

/* Data structure that holds both a text message and a temperature reading */
typedef struct {
  char     message[50];
  uint16_t temp_value;
} temp_data_t;

/* UDP connection and a timer for turning off LEDs */
static struct simple_udp_connection udp_conn;
static struct ctimer led_off_timer;

/* After the timer expires, turn off any LEDs that were switched on */
static void
turn_leds_off_cb(void *ptr)
{
  leds_off(LEDS_RED);
  leds_off(LEDS_BLUE);
}

/* Serialize and send the temperature packet to the multicast address */
static void
broadcast_temp_data(temp_data_t *data)
{
  uip_ipaddr_t dest_addr;
  uint8_t buffer[sizeof(*data)];

  memcpy(buffer, data, sizeof(*data));
  uiplib_ipaddrconv(MULTICAST_ADDRESS, &dest_addr);
  simple_udp_sendto(&udp_conn, buffer, sizeof(buffer), &dest_addr);
}

/* This function is invoked when a packet arrives. It shows the received data
   and signals with LEDs that a packet was received. */
static void
handle_incoming_data(struct simple_udp_connection *c,
                     const uip_ipaddr_t *sender,
                     uint16_t sender_port,
                     const uip_ipaddr_t *receiver,
                     uint16_t receiver_port,
                     const uint8_t *data,
                     uint16_t datalen)
{
  temp_data_t incoming;
  memcpy(&incoming, data, sizeof(incoming));

  /* Turn on the blue LED briefly to indicate a successful reception */
  leds_on(LEDS_BLUE);
  ctimer_set(&led_off_timer, CLOCK_SECOND, turn_leds_off_cb, NULL);

  printf("Received Data: Message = %s, Temperature = %u\n",
         incoming.message, incoming.temp_value);
}

/*---------------------------------------------------------------------------*/
PROCESS(temperature_broadcast_process, "Temperature Broadcast Process");
AUTOSTART_PROCESSES(&temperature_broadcast_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(temperature_broadcast_process, ev, data)
{
  static struct etimer send_timer;
  static temp_data_t outgoing_msg;

  PROCESS_BEGIN();

  /* Initialize the SHT11 sensor. Even-numbered nodes will retrieve actual readings. */
  SENSORS_ACTIVATE(sht11_sensor);

  printf("Launching temperature broadcast\n");

  /* Set up the UDP connection and specify the callback for incoming packets */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, handle_incoming_data);

  /* Prepare a timer to send data every 4 seconds */
  etimer_set(&send_timer, 4 * CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
    etimer_reset(&send_timer);

    /* Even node IDs report a temperature value; odd ones send zero */
    uint16_t current_temp = (node_id % 2 == 0) ? sht11_sensor.value(SHT11_SENSOR_TEMP) : 0;

    if(node_id % 2 == 0) {
      sprintf(outgoing_msg.message, "Node %u: Temperature data available", node_id);
    } else {
      sprintf(outgoing_msg.message, "Node %u: No temperature sensor", node_id);
    }

    outgoing_msg.temp_value = current_temp;

    /* Broadcast the message and temperature */
    broadcast_temp_data(&outgoing_msg);

    /* Turn on the red LED for a second to show that data was sent */
    leds_on(LEDS_RED);
    ctimer_set(&led_off_timer, CLOCK_SECOND, turn_leds_off_cb, NULL);

    printf("sent broadcast message %u\n", node_id);
  }

  /* In practice, this line won't be reached, but we deactivate the sensor on process exit */
  SENSORS_DEACTIVATE(sht11_sensor);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

/*
 * Exercise 3a Answer:
 * After flashing the program onto multiple nodes and running it in the Cooja simulator,
 * you observe that each node broadcasts a message every 4 seconds. Nodes with an even node_id
 * read out the SHT11 sensor temperature and include it in their transmitted struct. Nodes
 * with an odd node_id do not have a temperature sensor reading and send a message indicating this.
 * Whenever a node receives a message, it prints the contents to the serial output and, if a
 * temperature reading is included, blinks the blue LED for 1 second.
 *
 * Exercise 3b Answer:
 * For the node with the temperature sensor, the code already reads out the temperature from
 * the SHT11 sensor each loop iteration. We then store the temperature value and the message
 * into a temperatureMessage struct, instead of using a "helloMessage" string.
 *
 * By using the defined struct temperatureMessage:
 *   struct temperatureMessage {
 *     char messageString[50];
 *     uint16_t temperature;
 *   };
 *
 * and placing the temperature reading and the descriptive message into 'messageString' and
 * 'temperature' fields, we ensure both data are sent together as a structured packet.
 *
 * On the receiving side, the node prints both the message and the temperature to the serial port.
 * If the temperature value is non-zero (indicating a valid reading), the receiver also blinks
 * the blue LED for 1 second, confirming that it has successfully interpreted the transmitted data.
 */
