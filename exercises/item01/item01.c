
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
    leds_on(LEDS_GREEN);
    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

    leds_off(LEDS_GREEN);
    etimer_set(&timer_wait, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  PROCESS_END();
}

PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_startup, timer_wait;

  etimer_set(&timer_startup, CLOCK_SECOND * 30);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

  while (1)
  {
    printf("Hello, world\n");
    etimer_set(&timer_wait, CLOCK_SECOND * 10);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

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
// this is my answer

/* Exercise 1c: alter the program such that the node prints "Hello World" infinitely to the screen,
 * with the delay of 4 seconds in between each statement
 */
 // my answer: 
 // changed the hello_world_process code to: 
 //
 // PROCESS_THREAD(hello_world_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_wait;

//   while (1)
//   {
//     printf("Hello, world\n");
//     etimer_set(&timer_wait, CLOCK_SECOND * 4); <- changed the value to 4
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//   }

//   PROCESS_END();
// }
// with this modification, there is now a delay of 4 seconds in between each statement



/* Exercise 1d: alter the program such that the node stops printing "Hello World" after 10 times
 */

//  my answer : 
//   - introduced a count variable outside the function (putting it inside would not increment the variable, always be 1)
//   - changed the while argument from 1 to "count < 10", this would mean that after having 10 Hello, world statements, it would stop

//   changed the hello_world_process to:

// int count = 0;
//   PROCESS_THREAD(hello_world_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_wait;

//   while (count < 10)
//   {
//     printf("Hello, world\n");
//     etimer_set(&timer_wait, CLOCK_SECOND * 4);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//     count++;
//   }

//   PROCESS_END();
// }


/* Exercise 1e: alter the program such that the node turns on the blue led in each loop,
 * keeps it turned on for 1 second and then continues.
 */
 // modified the hello_world_process again so that it would blink for 1 second
 // I modified it in the hello_world_process so that it would blink for 1 second before printing the Hello world statement
 // I modified the hello_world_process code to this: 

// PROCESS_THREAD(hello_world_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_wait, led_timer;

//   while (1)
//   {
//     leds_on(LEDS_BLUE);
//     etimer_set(&led_timer, CLOCK_SECOND * 1);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&led_timer));
//     leds_off(LEDS_BLUE);

//     printf("Hello, world\n");
//     etimer_set(&timer_wait, CLOCK_SECOND * 4);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
//   }

//   PROCESS_END();
// }



/* Exercise 1f: separate the led blinking logic from the "Hello World"
 * printing logic: use two different processes, one that a) infinitely turns on and off the blue
 * LED (blink time = 1s, repeat interval=4s) and one that b) infinitely prints "Hello World"
 * with an interval of 10s
 *
 * The two processes shall be started when the node boots. The first (a) shall start immediately,
 * the second (b) shall start 30s after bootup.
 */

 // MY ANSWER: 
//   for this exercise, i just modified the original program given to us from "LEDS_BLUE" to "LEDS_GREEN" since it was already
//   implemented in the provided code: 

//   PROCESS_THREAD(led_blink_process, ev, data)
// {
//   PROCESS_BEGIN();
//   static struct etimer timer_blink, timer_wait;
//   // Blink pattern every 4 seconds

//   while (1)
//   {
//     leds_on(LEDS_GREEN);
//     etimer_set(&timer_blink, CLOCK_SECOND * 1);
//     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

//     leds_off(LEDS_GREEN);
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


