#include "contiki.h"
#include "net/ipv6/uiplib.h"
#include "simple-udp.h"
#include "dev/sht11-sensor.h"
#include <stdio.h>

#define SEND_INTERVAL (CLOCK_SECOND * 15)  // Send data every 15 seconds
#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // Link-local all nodes multicast address

static struct simple_udp_connection udp_conn;

struct temperatureMessage {
  uint16_t temperature;  // temperature in hundredths of degrees Celsius
};

PROCESS(source_mote_process, "Source Mote Process");
AUTOSTART_PROCESSES(&source_mote_process);

static void send_temperature_data(void) {
  uip_ipaddr_t broadcast_addr;

  // Initialize the broadcast address (link-local all nodes multicast)
  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);

  // Read raw temperature from SHT11
  int16_t raw_temp = sht11_sensor.value(SHT11_SENSOR_TEMP);
  if (raw_temp == -1) {
    printf("Error reading temperature sensor\n");
  } else {
    // Convert raw reading to Celsius:
    // T(°C) = -39.60 + 0.01 * raw_temp
    double temp_c = -39.60 + 0.01 * (double)raw_temp;

    // Convert to hundredths of a degree
    int16_t temp_hundredths = (int16_t)(temp_c * 100);

    struct temperatureMessage tm;
    tm.temperature = (uint16_t)temp_hundredths;

    simple_udp_sendto(&udp_conn, &tm, sizeof(tm), &broadcast_addr);

    int integer_part = temp_hundredths / 100;
    int decimal_part = temp_hundredths % 100;
    if (decimal_part < 0) {
      decimal_part = -decimal_part;
    }

    printf("Sent temperature: %d.%02dC\n", integer_part, decimal_part);
  }
}

PROCESS_THREAD(source_mote_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(sht11_sensor);

  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, NULL);

  static struct etimer et;
  etimer_set(&et, SEND_INTERVAL);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    send_temperature_data();  // Send temperature data every 15 seconds
    etimer_reset(&et);
  }

  PROCESS_END();
}
