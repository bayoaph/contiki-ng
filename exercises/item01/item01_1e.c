/* Philipp Hurni, University of Bern, December 2013      */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019 */
/* Modified by Kent Dico - 2024 ; Target=z1*/

#include "contiki.h"
#include "etimer.h"
#include <stdio.h>
#include <string.h>
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello World Timer Process");
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_led, timer_wait; // Added two timers for LED and delay

  while (1) // Infinite loop for Exercise 1e
  {
    leds_on(LEDS_RED); // Turn on the red LED
    etimer_set(&timer_led, CLOCK_SECOND * 1); // Keep LED on for 1 second
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_led)); // Wait for LED timer expiration
    leds_off(LEDS_RED); // Turn off the red LED

    printf("Hello, world\n"); // Print "Hello, world"
    etimer_set(&timer_wait, CLOCK_SECOND * 3); // Wait for remaining 3 seconds to ensure 4-second total delay
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait)); // Wait for delay timer expiration
  }

  PROCESS_END();
}

// autostart processes (can also be more than one)
AUTOSTART_PROCESSES(&hello_world_process);

/* Exercise 1e: alter the program such that the node turns on the red LED in each loop,
 * keeps it turned on for 1 second and then continues.
 * Implementation note: Add logic to control the LED state before and after printing "Hello World".
 * Observation:
 * The program turns on the red LED for 1 second before printing "Hello, world" and continues in a loop with a 4-second delay.
 */
