#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "dev/sensor/sht11/sht11-sensor.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>
#include "sys/node-id.h"

#define SEND_INTERVAL (15 * CLOCK_SECOND)
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1"

/* Structure to hold a message and raw temperature value */
struct temperatureMessage {
  char messageString[50];
  uint16_t temperature; // Raw temperature reading
};

PROCESS(temperature_source_process, "Temperature source process");
AUTOSTART_PROCESSES(&temperature_source_process);

static struct simple_udp_connection udp_conn;
static struct temperatureMessage tm;
static int16_t raw_temp;

/* Helper function to broadcast the temperature packet */
static void
send_broadcast_packet(struct temperatureMessage *payload)
{
  uip_ipaddr_t broadcast_addr;
  uint8_t buffer[sizeof(struct temperatureMessage)];

  // Serialize the structure into the buffer
  memcpy(buffer, payload, sizeof(struct temperatureMessage));

  // Convert textual IPv6 address to a binary form
  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

  // Send as a broadcast
  simple_udp_sendto(&udp_conn, buffer, sizeof(buffer), &broadcast_addr);
}

PROCESS_THREAD(temperature_source_process, ev, data)
{
  static struct etimer periodic_timer;

  PROCESS_BEGIN();

  // Activate temperature sensor
  SENSORS_ACTIVATE(sht11_sensor);

  // Initialize UDP connection for broadcasting
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

  // Set an event timer to trigger at regular intervals
  etimer_set(&periodic_timer, SEND_INTERVAL);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_reset(&periodic_timer);

    // Read the raw temperature from SHT11 sensor
    raw_temp = sht11_sensor.value(SHT11_SENSOR_TEMP);

    // Print the temperature in a readable format before sending
    printf("Broadcasting temperature: %d.%d°C\n", raw_temp / 10, raw_temp % 10);

    // Fill the structure with data
    tm.temperature = raw_temp;
    snprintf(tm.messageString, sizeof(tm.messageString), "From: Node %u", node_id);

    // Broadcast the serialized temperature data
    send_broadcast_packet(&tm);
  }

  // (Never reached in normal operation)
  SENSORS_DEACTIVATE(sht11_sensor);

  PROCESS_END();
}
