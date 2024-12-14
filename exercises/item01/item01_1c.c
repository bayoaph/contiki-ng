/* Philipp Hurni, University of Bern, December 2013      */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019 */
/* Modified by Kent Dico - 2024 ; Target=z1 */


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
  static struct etimer timer_wait;

  while (1) // Modified for Exercise 1c: Print "Hello World" infinitely with a 4-second interval
  {
    printf("Hello, world\n");
    etimer_set(&timer_wait, CLOCK_SECOND * 4); // Modified for Exercise 1c: Set 4-second interval
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
  }

  PROCESS_END();
}

// autostart processes (can also be more than one)
AUTOSTART_PROCESSES(&hello_world_process);

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
 * Observe what happens:
 * Upon compiling and flashing, the program waits for 2 seconds before printing "Hello, world" and then terminates.
 */

/* Exercise 1c: alter the program such that the node prints "Hello World" infinitely to the screen,
 * with the delay of 4 seconds in between each statement.
 * Observation:
 * The program prints "Hello, world" repeatedly every 4 seconds without stopping.
 */
