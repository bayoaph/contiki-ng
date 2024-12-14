/* Based on exercises by Philipp Hurni, University of Bern, December 2013 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include "etimer.h"
#include <string.h>
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "node-id.h" /* get a pointer to the own node id */
//#include "dev/zoul-sensors.h"
#include "dev/sht11/sht11-sensor.h"  /* for the temperature sensor */
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-ds6.h"



#define UDP_PORT 1234
#define BROADCAST_ADDR "ff02::1" // Link-local all nodes multicast address



struct temperatureMessage
{
  char messageString[50];
  uint16_t temperature;
};

static struct simple_udp_connection udp_conn;
static struct ctimer leds_off_timer_send;

/* Timer callback turns off the blue led */
static void timerCallback_turnOffLeds()
{
  leds_off(LEDS_GREEN);
}

static void send_broadcast_packet(struct temperatureMessage *payload)
{
  uip_ipaddr_t broadcast_addr;

  // Create a buffer to hold the serialized data
  uint8_t buffer[sizeof(struct temperatureMessage)];
  memcpy(buffer, payload, sizeof(struct temperatureMessage)); // Serialize

  uip_create_linklocal_allnodes_mcast(&broadcast_addr);

  simple_udp_sendto(&udp_conn, buffer, sizeof(buffer), &broadcast_addr);
}

// Callback function to handle incoming packets
static void udp_recv_callback(struct simple_udp_connection *c,
                              const uip_ipaddr_t *sender_addr,
                              uint16_t sender_port,
                              const uip_ipaddr_t *receiver_addr,
                              uint16_t receiver_port,
                              const uint8_t *data,
                              uint16_t datalen)
{
  struct temperatureMessage received_data;
  memcpy(&received_data, data, sizeof(struct temperatureMessage)); // Deserialize

  if (received_data.temperature == 0)
    // nods without sensors
    printf("Received Sensor Data: Message = %s, Temperature = %u\n",
         received_data.messageString, received_data.temperature);
  else{
  // nodes with a sensor; convert temperature 
      printf("Received Sensor Data: Message = %s, Temperature = %u.%03u\n",
          received_data.messageString, received_data.temperature / 1000, received_data.temperature % 1000);

  }
    
    
    leds_on(LEDS_GREEN);
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);


}

/*---------------------------------------------------------------------------*/
/* one timer struct declaration/instantiation */
static struct etimer et;
/* one temperatureMessage struct declaration/instantiation  */
static struct temperatureMessage tm;
/*---------------------------------------------------------------------------*/
PROCESS(broadcast_rssi_process, "Broadcast RSSI");
AUTOSTART_PROCESSES(&broadcast_rssi_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_rssi_process, ev, data)
{
  // PROCESS_EXITHANDLER(broadcast_close(&bc);)
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(sht11_sensor);

  printf("Starting broadcast\n");
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);


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

    /* send the packet */
    send_broadcast_packet(&tm);

    /* turn on blue led */
    leds_on(LEDS_GREEN);
    /* set the timer "leds_off_timer" to 1 second */
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
    printf("sent broadcast message\n");
  }

  SENSORS_DEACTIVATE(sht11_sensor);

  PROCESS_END();
}

/* Exercise 3a: flash the program to your nodes and observe what happens. Read the code and understand it thoroughly.

  ANSWER:
    I have used the previous version of the lab exercise since I had already started on it and this version was the one that worked. Additionally, I have used the sht11 sensor 
    instead of the zoul-sensors as the sht11 was the one available. 


    Every 4 seconds, a message if broadcasted for every even node. Only the even nodes gets the value of their temperature sensor, for the odd nodes they have  a temperature of 0.
    Meaning they do not have a sensor. As can be seen on on the broadcast_rssi_process loop: 

      uint16_t temperature = (node_id % 2 == 0) ? sht11_sensor.value(SHT11_SENSOR_TEMP) : 0;

      // Send a different message, depending on the existence of a temperature sensor
      if (node_id % 2 == 0)
        sprintf(tm.messageString, "Here is node %u, sending the temperature", node_id);
      else
        sprintf(tm.messageString, "Here is node %u, I don't have a sensor", node_id);

    The message is then passed to the structure tm, along side the temperature and passed as a broadcast packet:
      send_broadcast_packet(&tm);
    Additionally, the green led for each led is turned is blicked, and the "sent broadcast message" is also printed.
      leds_on(LEDS_GREEN);
      ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
      printf("sent broadcast message\n");

    In the send_broadcast_packet, a buffer was created to hold the serialized data and in this function itlinks the nodes in order to send the messages.
    
    To handle the incoming packet messages is the udp_recv_callback. It deserializes the received data and prints the message. I have added an if else statement to the print function in the function.
    As the temperature taken was not yet converted. 
      if (received_data.temperature == 0)
        printf("Received Sensor Data: Message = %s, Temperature = %u\n",
          received_data.messageString, received_data.temperature);
      else
        printf("Received Sensor Data: Message = %s, Temperature = %u.%03u\n",
            received_data.messageString, received_data.temperature / 1000, received_data.temperature % 1000);

}

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
 * ANSWER:
 * For sending the temperatureMessage, it is used by this following lines of code:
 *  // Send a different message, depending on the existence of a temperature sensor
    if (node_id % 2 == 0)
      sprintf(tm.messageString, "Here is node %u, sending the temperature", node_id);
    else
      sprintf(tm.messageString, "Here is node %u, I don't have a sensor", node_id);
    tm.temperature = temperature;

    Additionally, instead of the BLUE led, I used a GREEN one as the BLUE LED wont blink in cooja:

     leds_on(LEDS_GREEN);
    ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);

    As such the final code is: 

      //     struct temperatureMessage
      // {
      //   char messageString[50];
      //   uint16_t temperature;
      // };

      // static struct simple_udp_connection udp_conn;
      // static struct ctimer leds_off_timer_send;

      //
      // static void timerCallback_turnOffLeds()
      // {
      //   leds_off(LEDS_GREEN);
      // }

      // static void send_broadcast_packet(struct temperatureMessage *payload)
      // {
      //   uip_ipaddr_t broadcast_addr;

      //   // Create a buffer to hold the serialized data
      //   uint8_t buffer[sizeof(struct temperatureMessage)];
      //   memcpy(buffer, payload, sizeof(struct temperatureMessage)); // Serialize

      //   uip_create_linklocal_allnodes_mcast(&broadcast_addr);

      //   simple_udp_sendto(&udp_conn, buffer, sizeof(buffer), &broadcast_addr);
      // }

      // // Callback function to handle incoming packets
      // static void udp_recv_callback(struct simple_udp_connection *c,
      //                               const uip_ipaddr_t *sender_addr,
      //                               uint16_t sender_port,
      //                               const uip_ipaddr_t *receiver_addr,
      //                               uint16_t receiver_port,
      //                               const uint8_t *data,
      //                               uint16_t datalen)
      // {
      //   struct temperatureMessage received_data;
      //   memcpy(&received_data, data, sizeof(struct temperatureMessage)); // Deserialize

      //   if (received_data.temperature == 0)
      //     // nods without sensors
      //     printf("Received Sensor Data: Message = %s, Temperature = %u\n",
      //         received_data.messageString, received_data.temperature);
      //   else
      //     // nodes with a sensor; convert temperature 
      //     printf("Received Sensor Data: Message = %s, Temperature = %u.%03u\n",
      //         received_data.messageString, received_data.temperature / 1000, received_data.temperature % 1000);

          
      //     leds_on(LEDS_GREEN);
      //     ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);


      // }

      //
      // static struct etimer et;
      // 
      // static struct temperatureMessage tm;
      // 
      // PROCESS(broadcast_rssi_process, "Broadcast RSSI");
      // AUTOSTART_PROCESSES(&broadcast_rssi_process);
      // 
      // PROCESS_THREAD(broadcast_rssi_process, ev, data)
      // {
      //   // PROCESS_EXITHANDLER(broadcast_close(&bc);)
      //   PROCESS_BEGIN();

      //   SENSORS_ACTIVATE(sht11_sensor);

      //   printf("Starting broadcast\n");
      //   simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, udp_recv_callback);


      //       
      //       etimer_set(&et, 4 * CLOCK_SECOND);

      //   while (1)
      //   {
      //     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
      //     etimer_reset(&et);

      //     // Temperature of 0 means no sensor, checked using node_id an even number
      //     uint16_t temperature = (node_id % 2 == 0) ? sht11_sensor.value(SHT11_SENSOR_TEMP) : 0;

      //     // Send a different message, depending on the existence of a temperature sensor
      //     if (node_id % 2 == 0)
      //       sprintf(tm.messageString, "Here is node %u, sending the temperature", node_id);
      //     else
      //       sprintf(tm.messageString, "Here is node %u, I don't have a sensor", node_id);

      //     // Save the temperature into the struct
      //     tm.temperature = temperature;

      //     
      //     send_broadcast_packet(&tm);

      //     
      //     leds_on(LEDS_GREEN);
      //    
      //     ctimer_set(&leds_off_timer_send, CLOCK_SECOND, timerCallback_turnOffLeds, NULL);
      //     printf("sent broadcast message\n");
      //   }

      //   SENSORS_DEACTIVATE(sht11_sensor);

      //   PROCESS_END();
      // }
 *
 *
 */