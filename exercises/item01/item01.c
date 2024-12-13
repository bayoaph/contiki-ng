
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
    leds_on(LEDS_RED);
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
 * 
 * Answer:
 * The code in simulation printed "Hello World" every ten seconds,but it  started printing after 30 seconds. 
 */

/* Exercise 1b: uncomment the three lines with the timer, compile and flash it to the sensor.
 * Observe what happens
 * Observation:
 * After commenting the three line with timer, the LED light is not blinking it keeps turning on while the simulation is running, 
 * while it still prints "Hello World" for every ten seconds after the 30 seconds mark. There are some changes, the LED light blinks 
 * for every 3 seconds.
 */
// this is my answer


/* Exercise 1c: alter the program such that the node prints "Hello World" infinitely to the screen,
 * with the delay of 4 seconds in between each statement
 *After changing line 47 in the code "etimer_set(&timer_wait, CLOCK_SECOND * 10);" to "etimer_set(&timer_wait, CLOCK_SECOND * 4);""
 It prints "Hello World" for every 4 seconds, with that the  blink of LED light and "Hello World" run similarly for 4 seconds.
 */

/* Exercise 1d: alter the program such that the node stops printing "Hello World" after 10 times
By mdifying the code, the result, node stops printing "Hello World" after 10 times

  static int count = 0;

  etimer_set(&timer_startup, CLOCK_SECOND * 30);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

  while (1)
  {
    printf("Hello, world\n");
    count++;
    etimer_set(&timer_wait, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    if(count==10)
    {
      break;
    }
  }
 */

/* Exercise 1e: alter the program such that the node turns on the blue led in each loop,
 * keeps it turned on for 1 second and then continues.
 I add a new time, "timer_green" and since the simulator dont have the functionality of color BLUE I used LED GREEN

 static struct etimer timer_startup, timer_wait, timer_green;
 
...

  while (1)
  {
    printf("Hello, world\n");
    count++;

    leds_on(LEDS_GREEN); // Turn on the GREEN LED
    etimer_set(&timer_green, CLOCK_SECOND * 1); // Wait for 1 second
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_green));
   leds_off(LEDS_GREEN); // Turn off the GREEN LED

    etimer_set(&timer_wait, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    if(count==10)
    {
      break;
    }
  }

 */

/* Exercise 1f: separate the led blinking logic from the "Hello World"
 * printing logic: use two different processes, one that a) infinitely turns on and off the blue
 * LED (blink time = 1s, repeat interval=4s) and one that b) infinitely prints "Hello World"
 * with an interval of 10s
 *
 * The two processes shall be started when the node boots. The first (a) shall start immediately,
 * the second (b) shall start 30s after bootup.
 
PROCESS_THREAD(led_blink_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_blink, timer_wait;
  // Blink pattern every 4 seconds

  while (1)
  {
    leds_on(LEDS_RED);
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
 * 
 * 
 */