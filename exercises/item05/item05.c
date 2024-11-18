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
  
  static uint16_t localVariable = 100;  // Declare the variable as static

  etimer_set(&et, CLOCK_SECOND * 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  printf("A: ATTENTION: localVariable = %u", localVariable);
  printf(" (address=%p)\n", (void *)&localVariable);

  SENSORS_ACTIVATE(button_sensor);
  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
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
    printf("B: %u (address=%p)\n", randomNum, (void *)&randomNum);  // Corrected: Use %p for address
  }
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS(C_PROCESS, "C");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(C_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static uint16_t inc = 0;
  while (1)
  {
    if (inc % 10000 == 0)
      printf("C: i %u\n", inc);
    inc++;
    
    // Temporarily yield control to other protothreads
    PROCESS_PAUSE();
  }
  PROCESS_END();
}

AUTOSTART_PROCESSES(&A_PROCESS, &B_PROCESS, &C_PROCESS); // autostart processes

/*---------------------------------------------------------------------------*/

/* Exercise 5 (to be solved with ILIAS):

 * a) In the process A, a variable "localVariable" is declared and printed out 10s after
 * the node boots with the value 100.
 *
 * Node B keeps looping and printing random numbers.
 *
 * When you press the "user" button, the process A again prints the value of the "localVariable".
 *
 * What value does it have when you run the program a) before the button sensor event and b) after ? why is it like this? explain.
 *
 * Answer:
 * - Before the button sensor event, `localVariable` has the value `100` as it is initialized to `100`.
 * - After the button sensor event, if `localVariable` is not declared as `static`, its value is
 *   undefined. This is because it is a local variable, and when the function stack is unwound,
 *   its value may be overwritten or lost.
 *
 * What do you have to do in order to make sure that the value of the variable "localVariable" is always 100?
 *
 * Answer:
 * - To ensure that `localVariable` always has the value `100`, declare it as `static`.
 *   This way, the variable retains its value between function calls and retains its address
 *   in memory rather than being reallocated each time.
 *
 *
 * b) Protothread C is a computationally intensive task. It is already defined in the code above but not yet used.
 *
 * Let it start at boot time. Observe what happens.
 *
 * Answer:
 * - Protothread C eats up all CPU, as it keeps running without releasing control to the other processes.
 *   It keeps incrementing `inc` in an infinite loop without yielding.
 *
 * As a programmer, you can solve such tasks in separate protothreads, while still being able to serve other protothreads.
 *
 * Find a solution to let C use the CPU while A and B are still being served. Check your final code with a Cooja simulator.
 *
 * Answer:
 * - To let `C_PROCESS` use the CPU while still allowing processes `A` and `B` to run, 
 *   add `PROCESS_PAUSE()` to the while loop in `C_PROCESS`. This will allow C to yield the 
 *   CPU after each increment, thus allowing other processes to run.
 * - The final code is provided above with `PROCESS_PAUSE()` in `C_PROCESS` to ensure fair CPU sharing.
 */

