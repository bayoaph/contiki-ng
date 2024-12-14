#include "contiki.h"
#include <stdio.h>
#include <string.h>
#include "simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "net/netstack.h"
#include "dev/button-sensor.h" /* for the button sensor */
#include "../../main_contiki/arch/dev/sensor/sht11/sht11-sensor.h"

#define SEND_INTERVAL (CLOCK_SECOND * 15)  // send data every 15 seconds
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // link-local all nodes multicast address

static struct simple_udp_connection udp_conn;

// Define the temperature message struct
struct temperatureMessage {
    uint16_t temperature;
};

static void send_temperature_data(void) {
  static int temp;
  uip_ipaddr_t broadcast_addr;

  // comvert the broadcast address to a UIP address structure
  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

  temp = sht11_sensor.value(SHT11_SENSOR_TEMP); // Read temperature
  printf("Source: Sending temperature: %d\n", temp);

  struct temperatureMessage tm;
  tm.temperature = temp;

  // send data to broadcast address
  simple_udp_sendto(&udp_conn, &tm, sizeof(tm), &broadcast_addr);
  printf("Sent Temperature: %d\n", temp);
}


PROCESS_THREAD(source_mote_process, ev, data) {
  static struct etimer et;

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(sht11_sensor); // Activate the temperature sensor
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);
  etimer_set(&et, SEND_INTERVAL);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    send_temperature_data();
    etimer_reset(&et);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS(source_mote_process, "Source Mote Process");
AUTOSTART_PROCESSES(&source_mote_process);
/*---------------------------------------------------------------------------*/
