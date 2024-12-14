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
  static struct etimer timer_wait; // Timer for delays
  static int print_count = 0;      // Counter to track number of prints

  while (print_count < 10) // Print "Hello World" 10 times
  {
    printf("Hello, world\n");
    etimer_set(&timer_wait, CLOCK_SECOND * 4); // Wait for 4 seconds
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait)); // Wait until timer expires

    print_count++; // Increment the print counter
  }

  PROCESS_END();
}

// autostart processes (can also be more than one)
AUTOSTART_PROCESSES(&hello_world_process);

/* Exercise 1d: alter the program such that the node stops printing "Hello World" after 10 times
 * Implementation note: Introduce a counter to track the number of prints and exit the loop after 10 iterations.
 * Observation:
 * The program prints "Hello, world" 10 times, with a 4-second delay between each print, and then stops.
 */


