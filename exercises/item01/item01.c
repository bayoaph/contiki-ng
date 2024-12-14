
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
    leds_on(LEDS_YELLOW);
    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

    leds_off(LEDS_YELLOW);
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

// My answer:
// These three lines are already uncommented. However, if we backtrack, the three commented lines are:

//  leds_off(LEDS_YELLOW);
//  etimer_set(&timer_wait, CLOCK_SECOND * 3);
//  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));

// When commented, the led remains turned on while the program is running
// When uncommented, this mimics the a blinking behavior. Specifically, what happens is:
//     1. the led turns on for 1 second
//     2. the LED then turns off and remains off for 3 seconds
// and this process repeats continuosly, so it looks like the led is blinking.


/* Exercise 1c: alter the program such that the node prints "Hello World" infinitely to the screen,
 * with the delay of 4 seconds in between each statement

Modify the hello_world_process such that there is delay of 4 seconds in between each statement.

while (1)
  {
    printf("Hello, world\n");
    etimer_set(&timer_wait, CLOCK_SECOND * 4); // modified from 10 to 4 second delay
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }


 */

/* Exercise 1d: alter the program such that the node stops printing "Hello World" after 10 times

Modify hello_world_process such that the node stops printing "Hello World" after 10 times

static int counter = 0; // declared a counter variable

while (counter < 10) // stops after 10 times
  {
    printf("Hello, world\n");
    counter++;
    etimer_set(&timer_wait, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

 */

/* Exercise 1e: alter the program such that the node turns on the yellow led in each loop,
 * keeps it turned on for 1 second and then continues.

Modify hello_world_process such that turns on the yellow led in each loop, keeps it turned on for 1 second and then continues.

while (counter < 10)
  {
    leds_on(LEDS_YELLOW); // turn on the red LED
    etimer_set(&timer_led, CLOCK_SECOND * 1); // // wait for 1 second (LED stays on for 1 second)
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_led));
    leds_off(LEDS_YELLOW); // turn off the red LED

    printf("Hello, world\n");
    counter++;
    etimer_set(&timer_wait, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

 */

/* Exercise 1f: separate the led blinking logic from the "Hello World"
 * printing logic: use two different processes, one that a) infinitely turns on and off the yellow
 * LED (blink time = 1s, repeat interval=4s) and one that b) infinitely prints "Hello World"
 * with an interval of 10s
 *
 * The two processes shall be started when the node boots. The first (a) shall start immediately,
 * the second (b) shall start 30s after bootup.

In this exercise, the blinking logic and the printing logic was seperated into two processes, which are alreay done.
The current code simulates the expected behavior where 
  1. the first process blinks the yellow LED (1s on/ 3s off for a total of 4s interval)
  2. the second process prints "Hello World" every 10 seconds
  3. the first process starts immediately, while the second process starts 30 seconds after bootup.

PROCESS_THREAD(led_blink_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_blink, timer_wait;
  // Blink pattern every 4 seconds

  while (1)
  {
    leds_on(LEDS_YELLOW);
    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

    leds_off(LEDS_YELLOW);
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

 */