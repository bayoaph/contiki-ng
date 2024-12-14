
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

/* Exercise 1b: Answer

  the three supposedly commented lines are:

    leds_off(LEDS_GREEN);
    etimer_set(&timer_wait, CLOCK_SECOND * 3);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));

  When commented:
    - The LED remains turned on
  When uncommented:
    - the LED turns on for 1 second
    - the LED then turns off
    - the program waits for 3 seconds as repeat
    * This creates a blinking pattern

*/



/* Exercise 1c: alter the program such that the node prints "Hello World" infinitely to the screen,
 * with the delay of 4 seconds in between each statement
*/

/* Exercise 1c: Answer

  The following line is the one responsible for the delay of 10 seconds per printing of "Hello World":

    etimer_set(&timer_wait, CLOCK_SECOND * 10);

  To alter the program such that it has a delay of 4 seconds, we can change its time parameter to 'CLOCK_SECOND * 4'.

    while (1){
      printf("Hello, world\n");
      etimer_set(&timer_wait, CLOCK_SECOND * 4); // altered from 10 to 4
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    }

*/



/* Exercise 1d: alter the program such that the node stops printing "Hello World" after 10 times
 */

/* Exercise 1d: Answer

  First define a counter variable x:

    static int x = 0;

  Change the condition of the loop from infinite to only 10 times using the counter variable

    while (x < 10){
      printf("Hello, world\n");
      x++;
      etimer_set(&timer_wait, CLOCK_SECOND * 4); // altered from 10 to 4
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    }

*/

/* Exercise 1e: alter the program such that the node turns on the green led in each loop,
 * keeps it turned on for 1 second and then continues.
 */

/* Exercise 1e: Answer

  The following lines of code are used to do the following:
    
    leds_on(LEDS_GREEN);                                  -> turns on the Green LED
    etimer_set(&timer_led, CLOCK_SECOND * X);             -> set a time of X second/s
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_led)); -> waits until the given timer has expired
    leds_off(LEDS_GREEN);                                 -> turns off the Green LED
  
  To achieve the looped 1-second blinking of the GREEN LED, add the mentioned lines of code into the loop

    while (counter < 10)
    {
      leds_on(LEDS_GREEN);
      etimer_set(&timer_led, CLOCK_SECOND * 1);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_led));
      leds_off(LEDS_GREEN);

      printf("Hello, world\n");
      counter++;
      etimer_set(&timer_wait, CLOCK_SECOND * 4);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
    }

*/

/* Exercise 1f: separate the led blinking logic from the "Hello World"
 * printing logic: use two different processes, one that a) infinitely turns on and off the green
 * LED (blink time = 1s, repeat interval=4s) and one that b) infinitely prints "Hello World"
 * with an interval of 10s
 *
 * The two processes shall be started when the node boots. The first (a) shall start immediately,
 * the second (b) shall start 30s after bootup.
 */

/* Exercise 1f: Answer

  Here is the Process A (logic for GREEN LED):

    PROCESS_THREAD(led_blink_process, ev, data)
    {
      PROCESS_BEGIN();
      static struct etimer timer_blink, timer_wait;
      // Blink pattern every 4 seconds

      while (1)
      {
        leds_on(LEDS_GREEN);
        etimer_set(&timer_blink, CLOCK_SECOND * 1); // 1 second blink time
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

        leds_off(LEDS_GREEN);
        etimer_set(&timer_wait, CLOCK_SECOND * 3); // each loop interval is 4 seconds (this is then subtracted by the blink time)
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
      }

      PROCESS_END();
    }


  Here is the Process B (logic for printing "Hello, world\n"):

    PROCESS_THREAD(hello_world_process, ev, data)
    {
      PROCESS_BEGIN();
      static struct etimer timer_startup, timer_wait;

      etimer_set(&timer_startup, CLOCK_SECOND * 30); // boot time of 30 seconds
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_startup));

      while (1)
      {
        printf("Hello, world\n");
        etimer_set(&timer_wait, CLOCK_SECOND * 10); // each loop interval is 10 seconds
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_wait));
      }

      PROCESS_END();
    }

*/






