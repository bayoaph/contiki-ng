
/* Philipp Hurni, University of Bern, December 2013 	 */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019 */

#include "contiki.h"
#include "etimer.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h" /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(led_blink_process, "LED Blink Timer Process");
PROCESS(hello_world_process, "Hello World Timer Process");
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(led_blink_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_blink, timer_wait; // comment timer_wait in doing exercise 1b
  // Blink pattern every 4 seconds

  while (1)
  {
    leds_on(LEDS_YELLOW);
    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

    // uncomment
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
  static int count = 1;

  etimer_set(&timer_startup, CLOCK_SECOND * 30);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

  while (1)
  {
    //exercise 1e
    leds_on(LEDS_GREEN);
    etimer_set(&timer_wait, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    leds_off(LEDS_GREEN);

    printf("%d\n", count);
    printf("Hello, world\n");
    etimer_set(&timer_wait, CLOCK_SECOND * 4); // exercise C: change 10 to 4
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    count++;
    

    // exercise 1d
    // count++;
    if (count > 10) {
      break;
    }
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


/*
  Answer Exercise 1a:

*/


/* Exercise 1b: uncomment the three lines with the timer, compile and flash it to the sensor.
 * Observe what happens
 
  Answer Exercise 1b:

  Lines to uncomment on led_blink_process thread:
    leds_off(LEDS_YELLOW);
    etimer_set(&timer_wait, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));

  If we uncomment the 3 lines with the timer, the LED turns off for 3 seconds. This 3 lines makes the LED blink. 
  If we comment it back, the LED would continue to turn on until the program is stop. 
*/

/* Exercise 1c: alter the program such that the node prints "Hello World" infinitely to the screen,
 * with the delay of 4 seconds in between each statement

  Answer Exercise 1c:
  Under the hello_world_process thread, change the delay to 4 seconds. With this, the interval of printing is 4 seconds instead of 10 seconds.

  Modification:
    while (1)
    {
      printf("Hello, world\n");
      etimer_set(&timer_wait, CLOCK_SECOND * 4); // from 10 sec to 4 sec
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    }
 */

/* Exercise 1d: alter the program such that the node stops printing "Hello World" after 10 times

  Answer Exercise 1d:

  Modification on the while loop under the hello_world_process thread:
    while (1)
    {
      printf("%d\n", count); // debug purposes
      
      printf("Hello, world\n");
      etimer_set(&timer_wait, CLOCK_SECOND * 4); // exercise C: change 10 to 4
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    
      // exercise 1d
      count++;
      if (count > 10) {
        break;
      }
    }
 
    By modifying the while loop, the program will only continue to print "Hello World" until count reaches 10. 
    Count was initialized to 1 at the beginning of the thread.
 */

/* Exercise 1e: alter the program such that the node turns on the blue led in each loop,
 * keeps it turned on for 1 second and then continues.

  Answer Exercise 1e:
  Modification on the while loop under the hello_world_process thread:
    while (1)
    {
      //exercise 1e
      leds_on(LEDS_GREEN);
      etimer_set(&timer_wait, CLOCK_SECOND * 1);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
      leds_off(LEDS_GREEN);

      printf("Hello, world\n");
      etimer_set(&timer_wait, CLOCK_SECOND * 4); // exercise C: change 10 to 4
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
      }
    }

    The modification turns on the Green LED for 1 second and then turns it off afterward, while printing the "Hello World". 
    This continues until the loop or program is stopped. 
 */

/* Exercise 1f: separate the led blinking logic from the "Hello World"
 * printing logic: use two different processes, one that 
 a) infinitely turns on and off the blue LED (blink time = 1s, repeat interval=4s) 
 b) infinitely prints "Hello World"
 * with an interval of 10s
 *
 * The two processes shall be started when the node boots. The first (a) shall start immediately,
 * the second (b) shall start 30s after bootup.
 * 
 * Answer:
 * a. 
 * PROCESS_THREAD(led_blink_process, ev, data)
  {
    PROCESS_BEGIN();
    static struct etimer timer_blink, timer_wait; // comment timer_wait in doing exercise 1b
    // Blink pattern every 4 seconds

    while (1)
    {
      leds_on(LEDS_YELLOW);
      etimer_set(&timer_blink, CLOCK_SECOND * 1);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

      // uncomment
      leds_off(LEDS_YELLOW);
      etimer_set(&timer_wait, CLOCK_SECOND * 3);  // 3-second wait to complete 4-second interval
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    }

    PROCESS_END();
  }

    In here, the first process thread turns on the yellow LED for 1 second and then turns it off to wait 
    for 3 seconds before turning the LED on again. The 1 second plus the 3 seconds completes the 
    4-second interval. This process is repeatedly done in loop until the simulation is stopped or
    terminated.

   b.
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
 AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process); 
  // auto-starts the two processes a and b but b starts 30 secs after because of the 
  // 30-sec startup timer 

    In here, the second process thread involves printing "Hello, world" infinitely with 
    an interval of 10 seconds. Athough this process thread also starts automatically at 
    the start of the simulation, it only starts printing the string after 30 secs because 
    of the 30-second timer before the while loop. Hence, it has to wait for the timer to
    finish before printing. 



 *  
 */