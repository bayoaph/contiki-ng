#include "contiki.h"
#include "dev/temperature-sensor.h"
#include "arch/dev/sensor/sht11/sht11-sensor.h"
#include <stdio.h>
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uiplib.h"


/*---------------------------------------------------------------------------*/
//#define SEND_INTERVAL (CLOCK_SECONDS * 15)
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // Link-local broadcast address

static struct simple_udp_connection udp_conn;
/*---------------------------------------------------------------------------*/
PROCESS(source_process, "Source Mote Process");
AUTOSTART_PROCESSES(&source_process);
/*---------------------------------------------------------------------------*/

static void send_temperature_data(void) {
  static int temp;
  uip_ipaddr_t broadcast_addr;

  // comvert the broadcast address to a UIP address structure
  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

  temp = sht11_sensor.value(0); // Read temperature
  printf("Source: Sending temperature: %d\n", temp);

  
  char msg[32];
  snprintf(msg, sizeof(msg), "Temp:%d", temp);

  // send data to broadcast address
  simple_udp_sendto(&udp_conn, msg, strlen(msg), &broadcast_addr); 
}

PROCESS_THREAD(source_process, ev, data) {
  static struct etimer et;

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(sht11_sensor);
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

  while (1) {
    etimer_set(&et, CLOCK_SECOND * 15); // set timer for 15 seconds
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    send_temperature_data(); // send temperature data every 15 seconds
  }

  SENSORS_DEACTIVATE(sht11_sensor);

  PROCESS_END();
}