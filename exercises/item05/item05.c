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
  while (1)
  {
    if (inc % 10000 == 0)
      printf("C: i %u\n", inc);
    inc++;
    // Temporarily give the processor to another protothread
    PROCESS_PAUSE();
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
 * What do you have to do in order to make sure that the value of the variable "localVariable" since it is assigned to 100 in the code.
 * 
 * always 100?
 * 
 * ANSWER:
 *1. )  Clicking the button, the "localVariable" has the value = 100. The process A_PROCESS is designed to print the value of the variable 
      localVariable after 10 seconds from the node boot.
      After the button is pressed will still be 100 because the variable localVariable
     was not modified anywhere else in the code (no operations are performed on localVariable).
 *why is it like this? explain.
        The reason the value of localVariable remains 100, both before and after the button event,
         is that the variable is initialized once to 100 and never modified.

 * What do you have to do in order to make sure that the value of the variable "localVariable" since it is assigned to 100 in the code.
 To ensure localVariable always stays 100, declare it as a constant like this:
          const uint16_t localVariable = 100;
This way, its value cannot be changed anywhere in the program.
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
 * ANSWER:
 * The Protothread C is in the code already,
 * The PROCESS_PAUSE() call allows Protothread C to temporarily give up the CPU, 
 * letting the system schedule other processes (like A and B) to run.
 * 
 * The solution 
 * To ensure that Protothread C uses the CPU while A and B are still being served, you need to
 *  allow C to yield the CPU periodically. This will give A and B a chance to run, even when C is performing computationally intensive tasks.
  The solution is to modify Protothread C so that it yields control to the event handler,
 which allows other protothreads (like A and B) to be served. The key function to achieve this is PROCESS_PAUSE().
 *
 */