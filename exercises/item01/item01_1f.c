/* Philipp Hurni, University of Bern, December 2013      */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019 */
/* Modified by Kent Dico - 2024 ; Target=z1*/

#include "contiki.h"
#include "etimer.h"
#include <stdio.h>
#include <string.h>
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(led_blink_process, "LED Blink Timer Process"); 
PROCESS(hello_world_process, "Hello World Timer Process"); 
PROCESS(led_blink_blue_process, "LED Blink Blue Process"); 
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(led_blink_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_blink, timer_wait;
  // Blink pattern every 4 seconds

  while (1)
  {
    leds_on(LEDS_RED); // Modified for Exercise 1e: Changed LED to RED since BLUE does not light
    etimer_set(&timer_blink, CLOCK_SECOND * 1); // Modified for Exercise 1c: Added 1-second timer for LED on
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

    leds_off(LEDS_RED); // Modified for Exercise 1e: Changed LED to RED since BLUE does not light
    etimer_set(&timer_wait, CLOCK_SECOND * 3); // Modified for Exercise 1c: Added 3-second timer for LED off
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  PROCESS_END();
}

PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_wait;

  // Delay start of Hello World process by 30 seconds for Exercise 1f
  etimer_set(&timer_wait, CLOCK_SECOND * 30);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));

  while (1) // Modified for Exercise 1f: Print "Hello World" infinitely with a 10-second interval
  {
    printf("Hello, world\n");
    etimer_set(&timer_wait, CLOCK_SECOND * 10); // Modified for Exercise 1f: Set 10-second interval
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  PROCESS_END();
}

PROCESS_THREAD(led_blink_blue_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_blink;
  // Added for Exercise 1f: Blink blue LED every 4 seconds

  while (1)
  {
    leds_on(LEDS_BLUE); 
    etimer_set(&timer_blink, CLOCK_SECOND * 1); // Added for Exercise 1f: Keep blue LED on for 1 second
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

    leds_off(LEDS_BLUE); 
    etimer_set(&timer_blink, CLOCK_SECOND * 3); // Added for Exercise 1f: Wait for 3 seconds
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));
  }

  PROCESS_END();
}

// autostart processes (can also be more than one)
AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process, &led_blink_blue_process);

/* Exercise 1f: separate the LED blinking logic from the "Hello World"
 * printing logic: use two different processes, one that a) infinitely turns on and off the blue
 * LED (blink time = 1s, repeat interval=4s) and one that b) infinitely prints "Hello World"
 * with an interval of 10s
 *
 * The two processes shall be started when the node boots. The first (a) shall start immediately,
 * the second (b) shall start 30s after bootup.

 * Observation:
 * The red LED blinks every 4 seconds while "Hello, world" is printed every 10 seconds, with the processes running concurrently.
 */
