/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly and Contiki-NG - M. Cabilo, Apr 2019  */

#include "contiki.h"
#include "etimer.h"
#include <stdio.h> /* For printf() */
#include <string.h>
// #include "net/rime.h" - outdated
// #include "net/ipv6/simple-udp.h"
#include "simple-udp.h"
#include "net/ipv6/uiplib.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
// #include "dev/light.h"
#include "node-id.h" /* get a pointer to the own node id */
// #include "dev/zoul-sensors.h" 
#include "arch/dev/sensor/sht11/sht11-sensor.h"
#include "net/netstack.h"

struct temperatureMessage
{
  char messageString[50];
  uint16_t temperature;
};

#define UDP_PORT 1234
#define SEND_INTERVAL (10 * CLOCK_SECOND) // Interval to send broadcast message
#define BROADCAST_ADDR "ff02::1" // Link-local all nodes multicast address

void print_temperature_int_to_float(uint16_t temp)
{
  printf("%u.%03u", temp / 1000, temp % 1000);
}

static struct simple_udp_connection udp_connection;
static struct ctimer leds_off_timer_send;
static void timerCallback_turnOffLeds();
// Handle incoming packets
static void 
udp_recv_callback(struct simple_udp_connection *c,
                  const uip_ipaddr_t *sender_addr,
                  uint16_t sender_port,
                  const uip_ipaddr_t *receiver_addr,
                  uint16_t receiver_port,
                  const uint8_t *data,
                  uint16_t datalen) {

  struct temperatureMessage received_data;
  memcpy(&received_data, data, sizeof(struct temperatureMessage)); // Deserialize

  char sender_addr_str[UIPLIB_IPV6_MAX_STR_LEN];
  uiplib_ipaddr_snprint(sender_addr_str, sizeof(sender_addr_str), sender_addr);

  printf("Received Sensor Data: Message = %s, Temperature = %u\n", received_data.messageString, received_data.temperature);

  leds_on(LEDS_RED);
  ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
  
  };


/* Timer callback turns off the blue led */
static void timerCallback_turnOffLeds()
{
  leds_off(LEDS_GREEN);
}

static void broadcast_send(struct temperatureMessage *payload) {
  uip_ipaddr_t broadcast_addr;
  uint8_t buffer[sizeof(struct temperatureMessage)]; // Buffer to hold serialized data
  memcpy(buffer, payload, sizeof(struct temperatureMessage)); 
  uiplib_ipaddrconv(BROADCAST_ADDR, &broadcast_addr);
  simple_udp_sendto(&udp_connection, buffer, sizeof(buffer), &broadcast_addr);
}

/*---------------------------------------------------------------------------*/
// static const struct broadcast_callbacks broadcast_call = {recv_bc};
// static struct broadcast_conn bc;
/*---------------------------------------------------------------------------*/
/* one timer struct declaration/instantiation */
static struct etimer et;
/* one temperatureMessage struct declaration/instantiation  */
static struct temperatureMessage tm;
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_rssi_process, "Broadcast RSSI");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_rssi_process, ev, data)
{
  // PROCESS_EXITHANDLER(broadcast_close(&bc);)
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(sht11_sensor);

  printf("Starting broadcast\n");
  /* every broadcast has to be initiated with broadcast_open, where the broadcast_conn struct
   * are defined and the pointers to the callback, the function which has to be defined that is
   * going to be called when RECEIVING the broadcast
   */
  // broadcast_open(&bc, BROADCAST_CHANNEL, &broadcast_call);

  simple_udp_register(&udp_connection, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);

  /* Broadcast every 4 seconds */
  etimer_set(&et, 4 * CLOCK_SECOND);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et);

    // Temperature of 0 means no sensor, checked using node_id an even number
    uint16_t temperature = (node_id % 2 == 0) ? sht11_sensor.value(SHT11_SENSOR_TEMP) : 0;

    // Send a different message, depending on the existence of a temperature sensor
    if (node_id % 2 == 0)
      sprintf(tm.messageString, "Here is node %u, sending the temperature", node_id);
    else
      sprintf(tm.messageString, "Here is node %u, I don't have a sensor", node_id);

    // Save the temperature into the struct
    tm.temperature = temperature;

    // Copy the temperature struct in the packet buffer
    // packetbuf_copyfrom(&tm, sizeof(tm));

    /* send the packet */
    // broadcast_send(&bc);
    broadcast_send(&tm);

    /* turn on blue led */
    leds_on(LEDS_GREEN);
    /* set the timer "leds_off_timer" to 1 second */
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
    printf("sent broadcast message\n");
  }

  SENSORS_DEACTIVATE(sht11_sensor);

  PROCESS_END();
}
AUTOSTART_PROCESSES(&broadcast_rssi_process);

// static void
// recv_bc(struct broadcast_conn *c, rimeaddr_t *from)
// {
//   // Read the incoming data into the static temperatureMessage struct
//   packetbuf_copyto(&tm);

//   // Temperature = 0 means that we received a message from the sensor without the sht11 sensor
//   if (tm.temperature != 0)
//   {
//     printf("Temperature: ");
//     print_temperature_int_to_float(tm.temperature);
//     printf("\n");
//   }
//   printf("received '%s' from %d.%d, RSSI=%i\n", tm.messageString, from->u8[0], from->u8[1], (int)packetbuf_attr(PACKETBUF_ATTR_RSSI));
// }

// static void
// broadcast_recv(struct simple_udp_connection *c,
//                const uip_ipaddr_t *sender_addr,
//                uint16_t sender_port,
//                const uip_ipaddr_t *receiver_addr,
//                uint16_t receiver_port,
//                const uint8_t *data,
//                uint16_t datalen)
// {
//   LOG_INFO("Received broadcast from ");
//   LOG_INFO_6ADDR(sender_addr);
//   LOG_INFO_(": '%.*s'\n", datalen, (char *)data);
// }

/* Exercise 3a: flash the program to your nodes and observe what happens. Read the code and understand it thoroughly.
 */

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
