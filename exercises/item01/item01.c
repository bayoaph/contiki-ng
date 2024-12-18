#include "contiki.h"
#include "etimer.h"
#include <stdio.h>
#include <string.h>
#include "dev/leds.h"
/* If needed, include node-id.h for referencing node_id */
/* #include "node-id.h" */

/*---------------------------------------------------------------------------*/
PROCESS(led_blink_process, "LED Blink Timer Process");
PROCESS(hello_world_process, "Hello World Timer Process");
AUTOSTART_PROCESSES(&led_blink_process, &hello_world_process);
/*---------------------------------------------------------------------------*/

/* LED Blink Process:
 * - Starts immediately at boot
 * - Turns on green LED for 1 second
 * - Turns it off and waits 3 seconds
 * - Repeats indefinitely
 */
PROCESS_THREAD(led_blink_process, ev, data)
{
  PROCESS_BEGIN();
  
  static struct etimer timer_blink_on, timer_blink_off;

  while (1) {
    /* Turn LED on for 1s */
    leds_on(LEDS_GREEN);
    etimer_set(&timer_blink_on, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink_on));

    /* Turn LED off for 3s */
    leds_off(LEDS_GREEN);
    etimer_set(&timer_blink_off, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink_off));
  }

  PROCESS_END();
}

/* Hello World Process:
 * - Waits 30s after boot before starting
 * - Prints "Hello, world" 10 times
 * - Each print:
 *    * Turn on the green LED for 1 second
 *    * Wait an additional 3 seconds (total 4s per cycle)
 * - After 10 prints, stops printing
 */
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
  
  static struct etimer timer_startup, timer_wait;
  static int count = 0;
  
  /* Wait 30 seconds after boot */
  etimer_set(&timer_startup, CLOCK_SECOND * 30);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

  while (1) {
    if (count >= 10) {
      /* After 10 times, stop printing */
      break;
    }

    printf("Hello, world\n");
    count++;

    /* Turn LED on for 1 second */
    leds_on(LEDS_GREEN);
    etimer_set(&timer_wait, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    leds_off(LEDS_GREEN);

    /* Wait additional 3 seconds to make total wait 4s between prints */
    etimer_set(&timer_wait, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/