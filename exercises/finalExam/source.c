#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "dev/sht11/sht11-sensor.h"
#include "net/ipv6/uiplib.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h" /* get a pointer to the own node id */

#define UDP_PORT 1234
#define SINK_IP "fe80::212:7401:1:101"  // Sink IP Address

/*---------------------------------------------------------------------------*/
PROCESS(source_mote_process, "Source Mote Process");
AUTOSTART_PROCESSES(&source_mote_process);
/*---------------------------------------------------------------------------*/


//Temperature data to be passed
struct temperatureMessage
{
  uint16_t node;
  uint16_t temperature;
};

static struct simple_udp_connection udp_conn;
static struct temperatureMessage tm;
static struct etimer temp_timer;

//Sending Data to Sink
static void send_packet(struct temperatureMessage *payload)
{
  // Create a buffer to hold the serialized data
  uint8_t buffer[sizeof(struct temperatureMessage)];
  memcpy(buffer, payload, sizeof(struct temperatureMessage)); // Serialize

  uip_ipaddr_t sink_address;
  uiplib_ip6addrconv(SINK_IP, &sink_address);  // Convert the sink IP address

  // Send the raw data to the sink (unicast)
  simple_udp_sendto(&udp_conn, buffer, sizeof(buffer), &sink_address);
}

PROCESS_THREAD(source_mote_process, ev, data)
{
  

  PROCESS_BEGIN();

  // Initialize UDP connection
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

  SENSORS_ACTIVATE(sht11_sensor);

  // set timer
  etimer_set(&temp_timer, CLOCK_SECOND * 15);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&temp_timer));

    // get temp
    uint16_t temperature = sht11_sensor.value(SHT11_SENSOR_TEMP);
 
    // data packet to be sent
    tm.node = node_id;
    tm.temperature = temperature;

    // send  packet to sink
    send_packet(&tm);
    

    // reset the timer
    etimer_reset(&temp_timer);
  }

  PROCESS_END();
}
