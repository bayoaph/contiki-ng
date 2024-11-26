
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
    
    //uncomment this 
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
    etimer_set(&timer_wait, CLOCK_SECOND * 4);
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
 * 
 * ANSWER:
 * The Node kept printing every 10 seconds after the 30 second mark. There were no LED blinking as the LED was set on the green where cooja does not handle.
 */

/* Exercise 1b: uncomment the three lines with the timer, compile and flash it to the sensor.
 * Observe what happens
  
  OBSERVATION: 
  After commenting the lines :
    leds_off(LEDS_GREEN);
    etimer_set(&timer_wait, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));

  the LED would not blink, the LED would turn on and keep on during the duration of the run. The Displaying of Hello World keeps ..
  ..on being printed after 10 seconds. However, when the lines were uncommented, the LED would blink from the start for every 3 seconds.

 */
// this is my answer

/* Exercise 1c: alter the program such that the node prints "Hello World" infinitely to the screen,
 * with the delay of 4 seconds in between each statement
 
 To do this, the  loop would be changed to the code below, in order to print "Hello World" every 4 seconds under the hello_world_process
  Instead of etimer_set(&timer_wait, CLOCK_SECOND * 10);, it would be 
  etimer_set(&timer_wait, CLOCK_SECOND * 4);


  while (1)
  {
    printf("Hello, world\n");
    etimer_set(&timer_wait, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }
 */

/* Exercise 1d: alter the program such that the node stops printing "Hello World" after 10 times

  Under the hello_world_process, create a variable count which will count the number of times it prints hello world, and inside the ...
  .. loop, a condition would be added to exit the loop after 10 "Hello World"

  static int count =0; //initialize to 0

  ...
  while (1)
  {
    printf("Hello, world\n");
    etimer_set(&timer_wait, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    count++; //increment by 1
    if(count == 10) 
      break;
  }

 */

/* Exercise 1e: alter the program such that the node turns on the blue led in each loop,
 * keeps it turned on for 1 second and then continues.

  Since cooja does not have a blue LED, I have used the green LED. 
  Inside hello_world_process, I created a new timer "timer_light" to handle the LED turning on and off. 
  Inside the loop, the following codes were added: 
    ...
    etimer_set(&timer_wait, CLOCK_SECOND * 4);

    leds_on(LEDS_GREEN); //turns on green LED
    etimer_set(&timer_light, CLOCK_SECOND * 1); //keeps it turned on for 1 sec
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_light)); //stops the timer
    leds_off(LEDS_GREEN); //turns the LED off

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    ...

 */

/* Exercise 1f: separate the led blinking logic from the "Hello World"
 * printing logic: use two different processes, one that a) infinitely turns on and off the blue
 * LED (blink time = 1s, repeat interval=4s) and one that b) infinitely prints "Hello World"
 * with an interval of 10s
 *
 * The two processes shall be started when the node boots. The first (a) shall start immediately,
 * the second (b) shall start 30s after bootup.
 * 
 * Instead of the blue LED, I will use the yellow
 * 
 * PROCESS_THREAD(led_blink_process, ev, data)
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
    etimer_set(&timer_wait, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  PROCESS_END();
}

// autostart processes (can also be more than one)
AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);
 * 
 */