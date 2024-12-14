/* Source Node Code */
#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "sys/etimer.h"
#include <string.h>
#include "dev/leds.h"
#include "dev/temperature-sensor.h" // For onboard temperature sensor (Sky/Z1)
#include "net/ipv6/simple-udp.h"
#include "sys/node-id.h" 
#include "net/ipv6/uiplib.h"

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // IPv6 broadcast address

static struct simple_udp_connection udp_conn;
struct temperatureMessage {
  char messageString[50];
  uint16_t temperature;
};

static void send_broadcast_packet(struct temperatureMessage *payload) {
  uip_ipaddr_t broadcast_addr;

  // Serialize the structure
  uint8_t buffer[sizeof(struct temperatureMessage)];
  memcpy(buffer, payload, sizeof(struct temperatureMessage));

  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
  simple_udp_sendto(&udp_conn, buffer, sizeof(struct temperatureMessage), &broadcast_addr);
}

static struct etimer et;
static struct temperatureMessage tm;

PROCESS(source_node_process, "Source Node Process");
AUTOSTART_PROCESSES(&source_node_process);

PROCESS_THREAD(source_node_process, ev, data) {
  PROCESS_BEGIN();

  // Activate the onboard temperature sensor (For Sky/Z1)
  SENSORS_ACTIVATE(temperature_sensor);
  printf("Source node started\n");

  clock_wait(CLOCK_SECOND);  // Wait a moment before starting

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);
  etimer_set(&et, 15 * CLOCK_SECOND); // Transmit every 15 seconds

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);

    // Read the temperature from the onboard temperature sensor (sky mote)
    uint16_t temperature = temperature_sensor.value(0); // Value is in mV

    // Convert the raw value to in Celsius
    uint16_t readable_temp = (temperature - 270) / 10; 

    sprintf(tm.messageString, "Node %u: Sending temperature", node_id);
    tm.temperature = readable_temp;  // Use the converted temperature

    // Debug: Print the converted temperature before sending the broadcast
    printf("Node %u: Sending temperature %u.%uC\n", node_id, readable_temp / 10, readable_temp % 10);

    // Send the broadcast packet with the temperature data
    send_broadcast_packet(&tm);
  }

  // Deactivate the sensor when done
  SENSORS_DEACTIVATE(temperature_sensor);
  PROCESS_END();
}
