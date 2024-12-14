/* Philipp Hurni, University of Bern, December 2013     */
/* Exercises for sensor networks lecture                */

#include "contiki.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h" /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
#include "lib/random.h"

/*---------------------------------------------------------------------------*/
PROCESS(A_PROCESS, "A");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(A_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer periodic_timer;
  static struct etimer init_timer;

  /* Initial delay */
  etimer_set(&init_timer, CLOCK_SECOND * 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&init_timer));

  static uint16_t localVariable = 100;
  printf("A: ATTENTION: localVariable = %u", localVariable);
  printf(" (address=%p)\n", (void *)&localVariable);

  /* Periodic printing of localVariable instead of button press */
  etimer_set(&periodic_timer, CLOCK_SECOND * 15);
  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    printf("A: ATTENTION: localVariable = %u", localVariable);
    printf(" (address=%p)\n", (void *)&localVariable);
    etimer_reset(&periodic_timer);
  }

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
  static uint16_t inc = 0;
  while (1)
  {
    if (inc % 10000 == 0)
      printf("C: i %u\n", inc);
    inc++;
    PROCESS_PAUSE(); // Allow other processes to execute
  }
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
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
