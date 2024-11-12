/* Philipp Hurni, University of Bern, December 2013      */
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
  static struct etimer timer_blink, timer_wait;

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
  static int count = 0;

  // Exercise 1f: Start hello_world_process 30 seconds after bootup
  etimer_set(&timer_startup, CLOCK_SECOND * 30);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

  while (1)
  {
    // Exercise 1d: Stop printing after 10 times
    if (count >= 10)
    {
      break;
    }

    // Exercise 1c: Print "Hello World" infinitely with a 4-second interval
    printf("Hello, world\n");
    count++;

    // Exercise 1e: Turn on blue LED for 1 second each time "Hello World" is printed
    leds_on(LEDS_GREEN);
    etimer_set(&timer_wait, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    leds_off(LEDS_GREEN);

    // Wait for 3 more seconds (total delay 4 seconds)
    etimer_set(&timer_wait, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  PROCESS_END();
}

// autostart processes (can also be more than one)
AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);

/* Explanation for Each Exercise:
 * 
 * Exercise 1a: Compile the Contiki OS and flash the node attached to the PC via USB with
 * the above process. Follow the instructions in the comments.
 *
 * Exercise 1b: Uncomment the three lines with the timer in hello_world_process (they are uncommented by default here).
 *
 * Exercise 1c: The hello_world_process prints "Hello World" infinitely with a delay of 4 seconds between each print.
 *
 * Exercise 1d: The process is altered to stop printing after 10 times using a counter variable "count".
 *
 * Exercise 1e: Each time "Hello World" is printed, the blue LED is turned on for 1 second.
 *
 * Exercise 1f: The led_blink_process handles blinking independently, and the hello_world_process starts 30 seconds after boot.
 */
