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
#include "dev/sht25.h" /* For SHT25 temperature sensor */
#include "node-id.h" /* Get a pointer to the own node ID */

#define LOG_MODULE "Broadcast"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_PORT 1234
#define SEND_INTERVAL (10 * CLOCK_SECOND) // Interval to send broadcast message

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
/* Timer and struct declarations */
static struct etimer et;
static struct temperatureMessage tm;
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_rssi_process, "Broadcast RSSI");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_rssi_process, ev, data) {
  PROCESS_BEGIN();

  /* Initialize the UDP broadcast connection */
  simple_udp_register(&broadcast_connection, UDP_PORT, NULL, UDP_PORT, udp_receive_callback);

  /* Activate the SHT25 sensor */
  SENSORS_ACTIVATE(sht25);

  LOG_INFO("Starting UDP broadcast. Node ID: %u\n", node_id);

  /* Broadcast every 4 seconds */
  etimer_set(&et, 4 * CLOCK_SECOND);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);

    uint16_t temperature = 0;

    /* Even node IDs read temperature */
    if (node_id % 2 == 0) {
      temperature = sht25.value(SHT25_VAL_TEMP);
      if (temperature == SHT25_ERROR) {
        LOG_ERR("Failed to read temperature.\n");
        temperature = 0;
      }
      sprintf(tm.messageString, "Node %u has temp data.", node_id);
    } else {
      sprintf(tm.messageString, "Node %u has no temp data.", node_id);
    }

    tm.temperature = temperature;

    /* Send the struct as a broadcast */
    simple_udp_sendto(&broadcast_connection, &tm, sizeof(tm), NULL);

    /* Log the broadcasted message */
    LOG_INFO("Broadcasted message: %s ", tm.messageString);
    if (tm.temperature != 0) {
      LOG_INFO("Temp: ");
      print_temperature_int_to_float(tm.temperature);
      LOG_INFO_("\n");
    } else {
      LOG_INFO_("No temp data.\n");
    }

    /* Turn on yellow LED */
    leds_on(LEDS_YELLOW);
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
  }

  SENSORS_DEACTIVATE(sht25);

  PROCESS_END();
}
AUTOSTART_PROCESSES(&broadcast_rssi_process);

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

  /* Copy the received data into the local struct */
  memcpy(&received_tm, data, sizeof(received_tm));

  /* Log the received message */
  LOG_INFO("Received: '%s' from ", received_tm.messageString);
  LOG_INFO_6ADDR(sender_addr);
  if (received_tm.temperature != 0) {
    printf("Temp: ");
    print_temperature_int_to_float(received_tm.temperature);
    printf("\n");
  } else {
    LOG_INFO_("No temp data.\n");
  }

  /* Blink the yellow LED for 1 second */
  leds_on(LEDS_YELLOW);
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
}


/* Exercise 3a: flash the program to your nodes and observe what happens. Read the code and understand it thoroughly.
 */

/* Summary:
 * This code sends periodic UDP broadcasts with a message and temperature (if available).
 * Even nodes include temperature data; odd nodes send a generic message.
 * `simple-udp` handles sending and receiving packets.
 * LED gives feedback (blinks yellow on send); timer ensures it turns off.
 * Designed to test node communication and data sharing in Contiki-NG.
 */

/* Changes:
 * Replaced `zoul-sensors.h` with `sht25.h` for Z1 compatibility.
 * Switched from `light.h` to `light-sensor.h`.
 * Dropped Rime (outdated) and used `simple-udp` for networking.
 * Removed Rime functions like `broadcast_open` and `recv_bc`.
 * Added `udp_receive_callback` for handling incoming UDP messages.
 * Used `simple_udp_register` and `simple_udp_sendto` for broadcasting.
 * Changed temperature sensor logic to use `sht25.value(SHT25_VAL_TEMP)`.
 * Updated LED to YELLOW (blue does not blink), but still the interface shows BLUE for 1 second after sending.
 * Logging replaced with `LOG_INFO` macros (modern Contiki-NG style).
 */

/* Observations:
 * Broadcast messages are sent successfully by both nodes every 4 seconds.
 * Logs show "Sent broadcast message" as expected.
 * Blue LED turns on briefly when broadcasting, indicating successful transmission.
 * Both nodes initialize properly with unique IDs and IPv6 addresses.
 * No temperature data is observed in the log (likely due to simulation limitations).
 * Nodes appear to function independently without receiving or processing each other's messages.
 */


