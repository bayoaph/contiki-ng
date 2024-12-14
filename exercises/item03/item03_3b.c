/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */
/* Modified by Kent Dico, 2024. Target=z1 */

#include "contiki.h"
#include "etimer.h"
#include "sys/log.h"
#include <stdio.h>
#include <string.h>
#include "net/ipv6/simple-udp.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/sht25.h"
#include "node-id.h"

#define LOG_MODULE "Broadcast"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_PORT 1234
#define SEND_INTERVAL (10 * CLOCK_SECOND)

struct temperatureMessage {
  char messageString[50];
  uint16_t temperature;
};

static struct simple_udp_connection broadcast_connection;

void print_temperature_int_to_float(uint16_t temp) {
  printf("%u.%03u", temp / 1000, temp % 1000);
}

static void timerCallback_turnOffLeds();
static void udp_receive_callback(struct simple_udp_connection *c,
                                  const uip_ipaddr_t *sender_addr,
                                  uint16_t sender_port,
                                  const uip_ipaddr_t *receiver_addr,
                                  uint16_t receiver_port,
                                  const uint8_t *data,
                                  uint16_t datalen);

static struct ctimer leds_off_timer_send;

/* Timer callback turns off the yellow LED */
static void timerCallback_turnOffLeds() {
  leds_off(LEDS_YELLOW);
}

/*---------------------------------------------------------------------------*/
static struct etimer et;
static struct temperatureMessage tm;
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_temperature_process, "Broadcast Temperature");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_temperature_process, ev, data) {
  PROCESS_BEGIN();

  /* Initialize UDP broadcast */
  simple_udp_register(&broadcast_connection, UDP_PORT, NULL, UDP_PORT, udp_receive_callback);

  /* Activate sensor */
  SENSORS_ACTIVATE(sht25);

  /* Log Node ID */
  LOG_INFO("Node ID: %u\n", node_id);

  /* Broadcast every 4 seconds */
  etimer_set(&et, 4 * CLOCK_SECOND);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);

    uint16_t temperature = sht25.value(SHT25_VAL_TEMP);
    if (temperature == SHT25_ERROR) {
      LOG_ERR("Failed to read temperature.\n");
      sprintf(tm.messageString, "Node %u has no valid temp.", node_id);
      tm.temperature = 0;
    } else {
      LOG_INFO("Read temperature: %u.%03u\n", temperature / 1000, temperature % 1000);
      sprintf(tm.messageString, "Node %u Temp:", node_id);
      tm.temperature = temperature;
    }

    /* Broadcast data */
    simple_udp_sendto(&broadcast_connection, &tm, sizeof(tm), NULL);

    /* Log broadcast */
    LOG_INFO("Broadcasted message: %s Temp: %u.%03u\n", tm.messageString, tm.temperature / 1000, tm.temperature % 1000);

    /* Blink yellow LED */
    leds_on(LEDS_YELLOW);
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
  }

  SENSORS_DEACTIVATE(sht25);

  PROCESS_END();
}
AUTOSTART_PROCESSES(&broadcast_temperature_process);

/*---------------------------------------------------------------------------*/
/* UDP receive callback */
static void udp_receive_callback(struct simple_udp_connection *c,
                                  const uip_ipaddr_t *sender_addr,
                                  uint16_t sender_port,
                                  const uip_ipaddr_t *receiver_addr,
                                  uint16_t receiver_port,
                                  const uint8_t *data,
                                  uint16_t datalen) {
  struct temperatureMessage received_tm;

  memcpy(&received_tm, data, sizeof(received_tm));

  LOG_INFO("Received: '%s' from ", received_tm.messageString);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");

  if (received_tm.temperature != 0) {
    printf("Received temperature: ");
    print_temperature_int_to_float(received_tm.temperature);
    printf("\n");

    leds_on(LEDS_YELLOW);
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
  } else {
    LOG_INFO("Received message with no temperature data.\n");
  }
}

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

/* Changes:
 * Added temperature reading using SHT25 sensor.
 * Added `temperatureMessage` struct to include temperature and message.
 * Removed `node_id % 2` condition, all nodes attempt to read temperature.
 * Added error check for SHT25 sensor.
 * Broadcast includes temperature and node ID.
 * Receiver logs and prints temperature, blinks LED on reception.
 */

/* Observations:
 * Both nodes broadcast temperature every 4 seconds.
 * Temperatures are identical because of Cooja's shared environment.
 * Messages include node ID and temperature.
 * Received temperatures are logged and displayed properly.
 * Blue LED blinks instead of yellow because blue does not light up.
 * If temperature fails, "no valid temp" is broadcast.
 */