/* Philipp Hurni, University of Bern, December 2013     */
/* Exercises for sensor networks lecture                */

#include "contiki.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h" /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
#include "lib/random.h"
#include "dev/button-sensor.h" /* for the button sensor */
/*---------------------------------------------------------------------------*/
PROCESS(A_PROCESS, "A");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(A_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer et;

  etimer_set(&et, CLOCK_SECOND * 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  static uint16_t localVariable = 100;
  printf("A: ATTENTION: localVariable = %u", localVariable);
  printf(" (address=%p)\n", &localVariable);

  SENSORS_ACTIVATE(button_sensor);
  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    printf("A: ATTENTION: localVariable = %u", localVariable);
    printf(" (address=%p)\n", &localVariable);
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
    printf("B: %u (address=%p)\n", randomNum, &randomNum);
  }
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS(C_PROCESS, "C");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(C_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static uint16_t inc;
  static uint16_t yield_factor = 10; // Adjust interval as needed

  static struct etimer yield_timer;

  while (1)
  {
    if (inc % 10000 == 0)
      printf("C: i %u\n", inc);
    inc++;

    etimer_set(&yield_timer, CLOCK_SECOND / yield_factor); 
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&yield_timer));
  }
  PROCESS_END();
}

AUTOSTART_PROCESSES(&A_PROCESS, &B_PROCESS, &C_PROCESS); // autostart processes

/* Exercise 5 (to be solved with ILIAS):*/

/* Question:
 *
 * In the process A, a variable "localVariable" is declared and printed out 10s after
 * the node boots with the value 100.
 *
 * Node B keeps looping and printing random numbers
 *
 * When you press the "user" button, the process A again prints the value of the "localVariable"
 *
 * What value does it have when you run the program a) before the button sensor event and b) after ? why is it like this? explain.
 *
*/

/* Answer:

  A. (Before the button sensor event)
    - 100, "localVariable" is initialized with a value of 100.

  B. (After the button sensor event)
    - still 100, because it was never modified during the execution of the process after its initial assignment.

  Why?
    - The value of "localVariable" is not persistent across multiple invocations of the process or events. 
    - Every time the Process is executed, the "localVariable" are re-initialized.

*/ 


/* Question:
 *
 * What do you have to do in order to make sure that the value of the variable "localVariable" is
 * always 100?
 *
*/

/* Answer:

  You can set the localVariable as a const:
    const uint16_t localVariable = 100;

  By doing so, it becomes read-only and its value cannot be modified during program execution.

*/

/* Question:
 *
 * Protothread C is a computationally intensive task. It is already defined in the code above but not yet used.
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

/* Answer:

  By setting a timer for a for the protothread C, it has to wait for the timer to expire.
  This allows the other protothreads to be served.

    PROCESS_THREAD(C_PROCESS, ev, data)
    {
      PROCESS_BEGIN();
      static uint16_t inc;
      static uint16_t yield_factor = 10; // Adjust interval as needed

      static struct etimer yield_timer;

      while (1)
      {
        if (inc % 10000 == 0)
          printf("C: i %u\n", inc);
        inc++;

        etimer_set(&yield_timer, CLOCK_SECOND / yield_factor); 
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&yield_timer));
      }
      PROCESS_END();
    }

*/