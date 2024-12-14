#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "dev/sensor/sht11/sht11-sensor.h"
#include <stdio.h>
#include "sys/node-id.h" 

#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1"

static struct simple_udp_connection udp_conn;
static int16_t temp;
struct temperatureMessage
{
  char messageString[50];
  uint16_t temperature;
};

static void send_broadcast_packet(struct temperatureMessage *payload)
{
  uip_ipaddr_t broadcast_addr;

  // Create a buffer to hold the serialized data
  uint8_t buffer[sizeof(struct temperatureMessage)];
  memcpy(buffer, payload, sizeof(struct temperatureMessage)); // Serialize

  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
  simple_udp_sendto(&udp_conn, buffer, sizeof(struct temperatureMessage), &broadcast_addr);
}

static struct temperatureMessage tm;

/*---------------------------------------------------------------------------*/
PROCESS(source_process, "Source mote process");
AUTOSTART_PROCESSES(&source_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(source_process, ev, data)
{
  static struct etimer periodic_timer;

  PROCESS_BEGIN();

  /* Register the UDP connection */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

  /* Activate the temperature sensor */
  SENSORS_ACTIVATE(sht11_sensor);

  /* Periodic timer to send temperature data every 15 seconds */
  etimer_set(&periodic_timer, CLOCK_SECOND * 15);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    /* Read temperature from the sensor */
    temp = sht11_sensor.value(SHT11_SENSOR_TEMP);
    printf("Sending temperature: %d.%d°C\n", temp / 10, temp % 10);
    tm.temperature = temp;
    sprintf(tm.messageString, "From: Node %u", node_id);
    /* Send temperature data as a broadcast */
    send_broadcast_packet(&tm);

    /* Reset the timer */
    etimer_reset(&periodic_timer);
  }

  SENSORS_DEACTIVATE(sht11_sensor);

  PROCESS_END();
}
