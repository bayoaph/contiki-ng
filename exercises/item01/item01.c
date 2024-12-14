
/* Philipp Hurni, University of Bern, December 2013 	 */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019 */

#include "contiki.h"
#include "etimer.h"
#include <stdio.h>
#include <string.h>
// #include "node-id.h" /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(led_blink_process, "LED Blink Timer Process");
PROCESS(hello_world_process, "Hello World Timer Process");
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(led_blink_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_blink, timer_wait;
  // Blink pattern every 4 seconds

  while (1)
  {
    leds_on(LEDS_RED); // initially LEDS_BLUE but cooja does not have it. Thus. changed to LEDS_RED
    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

    leds_off(LEDS_RED);                                   
    etimer_set(&timer_wait, CLOCK_SECOND * 3);             
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait)); 
  }

  PROCESS_END();
}

PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_startup, timer_wait;
  static int hello_world_counter = 0;


  etimer_set(&timer_startup, CLOCK_SECOND * 30);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

  // while(1)
  while (hello_world_counter < 10)
  {
    printf("Hello, world\n");
    hello_world_counter++;
    etimer_set(&timer_wait, CLOCK_SECOND * 10);
    // etimer_set(&timer_wait, CLOCK_SECOND * 4); // Changed from CLOCK_SECOND * 10 tO CLOCK_SECOND * 4 for Exercise 1c 
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  printf("Stopped printing Hello World");

  PROCESS_END();
}

// autostart processes (can also be more than one)
AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);

/* Exercise 1a: compile the Contiki OS and flash the node attached to the PC via USB with
 * the above process it usually works best with flashing when you are the superuser
 *
 * $ cd <item01-path>
 * $ make
 * $ make item01.upload
 *
 * then run the following command
 * $ make login
 *
 * you can now communicate with the sensor node over the USB interface
 * restart the sensor node with the reset button until you see
 * "Hello World" printed onto the screen
 */

/* Exercise 1b: uncomment the three lines with the timer, compile and flash it to the sensor.
 * Observe what happens
 */
/**************************************************************************************************
  This change retained the process of printing "Hello World" every 10 seconds after the initial 
  30-second wait time. In addition to this, by uncommenting the three lines, the LED can now be
  turned off for 3 seconds. Now, immediately after starting, it turns on for 1 second and turns off
  for seconds. Moreover, since there is no blue LED in Cooja, I changed it to red.

// #include "contiki.h"
// #include "etimer.h"
// #include <stdio.h>
// #include <string.h>
// // #include "node-id.h" /* get the variable node_id that holds the own node id */
// #include "dev/leds.h"
// /*---------------------------------------------------------------------------*/
// PROCESS(led_blink_process, "LED Blink Timer Process");
// PROCESS(hello_world_process, "Hello World Timer Process");
// /*---------------------------------------------------------------------------*/

// PROCESS_THREAD(led_blink_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_blink, timer_wait;
//   // Blink pattern every 4 seconds

//   while (1)
//   {
//     leds_on(LEDS_RED); // initially LEDS_BLUE but cooja does not have it. Thus. changed to LEDS_RED
//     etimer_set(&timer_blink, CLOCK_SECOND * 1);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

//     leds_off(LEDS_RED);                                   
//     etimer_set(&timer_wait, CLOCK_SECOND * 3);             
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait)); 
//   }

//   PROCESS_END();
// }

// PROCESS_THREAD(hello_world_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_startup, timer_wait;


//   etimer_set(&timer_startup, CLOCK_SECOND * 30);
//   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

//   while (1)
//   {
//     printf("Hello, world\n");
//     etimer_set(&timer_wait, CLOCK_SECOND * 10);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//   }


//   PROCESS_END();
// }

// // autostart processes (can also be more than one)
// AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);

//  **************************************************************************************************/
  

/* Exercise 1c: alter the program such that the node prints "Hello World" infinitely to the screen,
 * with the delay of 4 seconds in between each statement
 */
 /**************************************************************************************************
  This change can be done by changing etimer_set(&timer_wait, CLOCK_SECOND * 10); into 
  etimer_set(&timer_wait, CLOCK_SECOND * 4);. Nothing much changes with this change of value.
  It starts printing "Hello World" after the intial 30-second wait time and continues to print it
  after succeeding 4 seconds.

  //   #include "contiki.h"
// #include "etimer.h"
// #include <stdio.h>
// #include <string.h>
// // #include "node-id.h" /* get the variable node_id that holds the own node id */
// #include "dev/leds.h"
// /*---------------------------------------------------------------------------*/
// PROCESS(led_blink_process, "LED Blink Timer Process");
// PROCESS(hello_world_process, "Hello World Timer Process");
// /*---------------------------------------------------------------------------*/

// PROCESS_THREAD(led_blink_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_blink, timer_wait;
//   // Blink pattern every 4 seconds

//   while (1)
//   {
//     leds_on(LEDS_RED); // initially LEDS_BLUE but cooja does not have it. Thus. changed to LEDS_RED
//     etimer_set(&timer_blink, CLOCK_SECOND * 1);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

//     leds_off(LEDS_RED);                                   
//     etimer_set(&timer_wait, CLOCK_SECOND * 3);             
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait)); 
//   }

//   PROCESS_END();
// }

// PROCESS_THREAD(hello_world_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_startup, timer_wait;
//   static int hello_world_counter = 0;


//   etimer_set(&timer_startup, CLOCK_SECOND * 30);
//   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

//   while (1)
//   {
//     printf("Hello, world\n");
//     etimer_set(&timer_wait, CLOCK_SECOND * 4); // Changed from CLOCK_SECOND * 10 tO CLOCK_SECOND * 4 for Exercise 1c 
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//   }

//   printf("Stopped printing Hello World");

//   PROCESS_END();
// }

// // autostart processes (can also be more than one)
// AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);
//  **************************************************************************************************/

/* Exercise 1d: alter the program such that the node stops printing "Hello World" after 10 times
 */
 */
 //   #include "contiki.h"
// #include "etimer.h"
// #include <stdio.h>
// #include <string.h>
// // #include "node-id.h" /* get the variable node_id that holds the own node id */
// #include "dev/leds.h"
// /*---------------------------------------------------------------------------*/
// PROCESS(led_blink_process, "LED Blink Timer Process");
// PROCESS(hello_world_process, "Hello World Timer Process");
// /*---------------------------------------------------------------------------*/

// PROCESS_THREAD(led_blink_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_blink, timer_wait;
//   // Blink pattern every 4 seconds

//   while (1)
//   {
//     leds_on(LEDS_RED); // initially LEDS_BLUE but cooja does not have it. Thus. changed to LEDS_RED
//     etimer_set(&timer_blink, CLOCK_SECOND * 1);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

//     leds_off(LEDS_RED);                                   
//     etimer_set(&timer_wait, CLOCK_SECOND * 3);             
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait)); 
//   }

//   PROCESS_END();
// }

// PROCESS_THREAD(hello_world_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_startup, timer_wait;
//   static int hello_world_counter = 0;


//   etimer_set(&timer_startup, CLOCK_SECOND * 30);
//   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

//   // while(1)
//   while (hello_world_counter < 10)
//   {
//     printf("Hello, world\n");
//     hello_world_counter++;
    // //etimer_set(&timer_wait, CLOCK_SECOND * 10);
//     etimer_set(&timer_wait, CLOCK_SECOND * 4); // Changed from CLOCK_SECOND * 10 tO CLOCK_SECOND * 4 for Exercise 1c 
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//   }


//   PROCESS_END();
// }

// // autostart processes (can also be more than one)
// AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);
//  **************************************************************************************************/

/* Exercise 1e: alter the program such that the node turns on the blue led in each loop,
 * keeps it turned on for 1 second and then continues.
 */
  /*************************************************************************************************
  This has already been done by the intitial code.

//   #include "contiki.h"
// #include "etimer.h"
// #include <stdio.h>
// #include <string.h>
// // #include "node-id.h" /* get the variable node_id that holds the own node id */
// #include "dev/leds.h"
// /*---------------------------------------------------------------------------*/
// PROCESS(led_blink_process, "LED Blink Timer Process");
// PROCESS(hello_world_process, "Hello World Timer Process");
// /*---------------------------------------------------------------------------*/

// PROCESS_THREAD(led_blink_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_blink, timer_wait;
//   // Blink pattern every 4 seconds

//   while (1)
//   {
//     leds_on(LEDS_RED); // initially LEDS_BLUE but cooja does not have it. Thus. changed to LEDS_RED
//     etimer_set(&timer_blink, CLOCK_SECOND * 1);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

//     leds_off(LEDS_RED);                                   
//     etimer_set(&timer_wait, CLOCK_SECOND * 3);             
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait)); 
//   }

//   PROCESS_END();
// }

// PROCESS_THREAD(hello_world_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_startup, timer_wait;
//   static int hello_world_counter = 0;


//   etimer_set(&timer_startup, CLOCK_SECOND * 30);
//   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

//   // while(1)
//   while (hello_world_counter < 10)
//   {
//     printf("Hello, world\n");
//     hello_world_counter++;
    // //etimer_set(&timer_wait, CLOCK_SECOND * 10);
//     etimer_set(&timer_wait, CLOCK_SECOND * 4); // Changed from CLOCK_SECOND * 10 tO CLOCK_SECOND * 4 for Exercise 1c 
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//   }


//   PROCESS_END();
// }

// // autostart processes (can also be more than one)
// AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);
//  **************************************************************************************************/

/* Exercise 1f: separate the led blinking logic from the "Hello World"
 * printing logic: use two different processes, one that a) infinitely turns on and off the blue
 * LED (blink time = 1s, repeat interval=4s) and one that b) infinitely prints "Hello World"
 * with an interval of 10s
 *
 * The two processes shall be started when the node boots. The first (a) shall start immediately,
 * the second (b) shall start 30s after bootup.
 */

  /*************************************************************************************************
  This has already been done by the intitial code. Just reverted interval of printinh "Hello World"
  from printing in a 4-second delay to a 10-second delay.

  //   #include "contiki.h"
// #include "etimer.h"
// #include <stdio.h>
// #include <string.h>
// // #include "node-id.h" /* get the variable node_id that holds the own node id */
// #include "dev/leds.h"
// /*---------------------------------------------------------------------------*/
// PROCESS(led_blink_process, "LED Blink Timer Process");
// PROCESS(hello_world_process, "Hello World Timer Process");
// /*---------------------------------------------------------------------------*/

// PROCESS_THREAD(led_blink_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_blink, timer_wait;
//   // Blink pattern every 4 seconds

//   while (1)
//   {
//     leds_on(LEDS_RED); // initially LEDS_BLUE but cooja does not have it. Thus. changed to LEDS_RED
//     etimer_set(&timer_blink, CLOCK_SECOND * 1);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

//     leds_off(LEDS_RED);                                   
//     etimer_set(&timer_wait, CLOCK_SECOND * 3);             
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait)); 
//   }

//   PROCESS_END();
// }

// PROCESS_THREAD(hello_world_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_startup, timer_wait;
//   static int hello_world_counter = 0;


//   etimer_set(&timer_startup, CLOCK_SECOND * 30);
//   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

//   // while(1)
//   while (hello_world_counter < 10)
//   {
//     printf("Hello, world\n");
//     hello_world_counter++;
//     etimer_set(&timer_wait, CLOCK_SECOND * 10);
//     // etimer_set(&timer_wait, CLOCK_SECOND * 4); // Changed from CLOCK_SECOND * 10 tO CLOCK_SECOND * 4 for Exercise 1c 
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//   }

//   printf("Stopped printing Hello World");

//   PROCESS_END();
// }

// // autostart processes (can also be more than one)
// AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);
//  **************************************************************************************************/