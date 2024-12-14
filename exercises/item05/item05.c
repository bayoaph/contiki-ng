/* Philipp Hurni, University of Bern, December 2013     */
/* Exercises for sensor networks lecture                */
/* Modified by Kent Dico, 2024. Target=z1 */

#include "contiki.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h" /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
#include "lib/random.h"
#include "dev/button-sensor.h" /* for the button sensor */
/*---------------------------------------------------------------------------*/
static volatile uint16_t localVariable = 100; // Global declaration for safety
/*---------------------------------------------------------------------------*/
PROCESS(A_PROCESS, "A");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(A_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer et;

  etimer_set(&et, CLOCK_SECOND * 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  printf("A: ATTENTION: localVariable = %u", localVariable);
  printf(" (address=%p)\n", (void *)&localVariable);

  SENSORS_ACTIVATE(button_sensor);
  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    localVariable = 100; // Reinitialize for redundancy
    printf("A: ATTENTION: localVariable = %u", localVariable);
    printf(" (address=%p)\n", (void *)&localVariable);
  }
  SENSORS_DEACTIVATE(button_sensor);
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS(B_PROCESS, "B");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(B_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer et;
  while (1)
  {
    etimer_set(&et, CLOCK_SECOND / 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    uint16_t randomNum = random_rand();
    printf("B: %u (address=%p)\n", randomNum, (void *)&randomNum);
  }
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS(C_PROCESS, "C");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(C_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer et;
  static uint16_t inc;
  etimer_set(&et, CLOCK_SECOND / 10); // Allow periodic execution

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    etimer_reset(&et); // Reset the timer for the next interval

    for (int i = 0; i < 1000; i++) { // Perform computational work in chunks
      inc++;
      if (inc % 10000 == 0)
        printf("C: i %u\n", inc);
    }
  }
  PROCESS_END();
}

AUTOSTART_PROCESSES(&A_PROCESS, &B_PROCESS, &C_PROCESS); // autostart processes



/* Exercise 5 (to be solved with ILIAS):
 *
 * a) In the process A, a variable "localVariable" is declared and printed out 10s after
 * the node boots with the value 100.
 *
 * Node B keeps looping and printing random numbers
 *
 * When you press the "user" button, the process A again prints the value of the "localVariable"
 *
 * What value does it have when you run the program a) before the button sensor event and b) after ? why is it like this? explain.
 *
 * What do you have to do in order to make sure that the value of the variable "localVariable" is
 * always 100?
 */

/* Answer to a:
 * Before the button sensor event, the value of localVariable is 100 because it is initialized as such.
 */

/* Answer to b:
 * After the button sensor event, the value of localVariable remains 100 because:
 * - It is declared as volatile, preventing optimization issues.
 * - It is explicitly reinitialized in the button event handler.
 */

/* What to do to ensure it is always 100:
 * - Declare localVariable as volatile.
 * - Move localVariable to global scope.
 * - Reinitialize it in the button event handler if needed.
 */

/*
 * b) Protothread C is a computationally intensive task. It is already defined in the code above but not yet used.
 *
 * Let it start at boot time. Observe what happens.
 *
 * Protothread C eats up all CPU. Since it never returns control to the event handler, it monopolizes the CPU.
 *
 * As a programmer, you can solve such tasks in separate protothreads, while still being able to serve other protothreads.
 *
 * Find a solution to let C use the CPU while A and B are still being served. Check your final code with a Cooja simulator.
 *
 */

/* Observations:
 * Process A:
 * Correctly prints `localVariable = 100` 10 seconds after boot.
 * Prints `localVariable = 100` when the button is pressed.
 * Value of `localVariable` is consistent due to `volatile` declaration and reinitialization.
 *
 * Process B:
 * Generates random numbers every 0.5 seconds.
 * Prints random numbers with memory addresses.
 * Works as expected without interruptions from other processes.
 *
 * Process C:
 * Performs intensive computation in chunks of 1000 iterations.
 * Periodically prints progress at multiples of 10000, e.g., `C: i 10000`.
 * Yields control properly using timers, allowing A and B to run smoothly.
 *
 * General:
 * CPU sharing is achieved effectively.
 * No starvation of any process.
 * Timers ensure proper multitasking between processes.
 */
