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
  static unsigned localVariable = 100;
  printf("A: ATTENTION: localVariable = %u", localVariable);
  printf(" (address=%p)\n", (void*) &localVariable);

  SENSORS_ACTIVATE(button_sensor);
  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    printf("A: ATTENTION: localVariable = %u", localVariable);
    printf(" (address=%p)\n", (void*) &localVariable);
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
    unsigned randomNum = random_rand();
    printf("B: %u (address=%p)\n", randomNum, (void *) &randomNum);
  }
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS(C_PROCESS, "C");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(C_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static unsigned inc;
  static struct etimer et;

  while (1)
  {
    if (inc % 10000 == 0)
      printf("C: i %u\n", inc);
    inc++;

    // Add delay to allow other processes to run
    etimer_set(&et, CLOCK_SECOND / 100); 
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et)); 

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
 * 
 * ANSWER:
 * The localVariable is declared as static and initialized to 100 in process A. 
 * Before button event, it prints 100 because static makes sure that it's initialized once and retains its value.
 * Furthermore, after the button event, the value is still 100 since the code does not modify it.
 * 
 * Ensuring localVariable, declaring it as static const is recommended to avoid accidental changesS
 *
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