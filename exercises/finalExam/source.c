#include "contiki.h"
#include "dev/sht11/sht11-sensor.h"
#include "simple-udp.h"
#include <stdio.h>
#include "net/ipv6/uiplib.h"

PROCESS(source_process, "Source Process");
AUTOSTART_PROCESSES(&source_process);

static struct simple_udp_connection udp_conn;

static void send_temperature(void *ptr) {
  int temp = sht11_sensor.value(SHT11_SENSOR_TEMP);
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "Temperature: %d", temp);
  uip_ipaddr_t dest_ip;
  uiplib_ipaddrconv("ff02::1", &dest_ip); // Send to all nodes
  simple_udp_sendto(&udp_conn, buffer, sizeof(buffer), &dest_ip);
  printf("Sent temperature: %d\n", temp);
}

PROCESS_THREAD(source_process, ev, data) {
  static struct etimer et;
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(sht11_sensor);
  simple_udp_register(&udp_conn, 1234, NULL, 1234, NULL);

  while (1) {
    etimer_set(&et, CLOCK_SECOND * 15);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    send_temperature(NULL);
  }

  PROCESS_END();
}