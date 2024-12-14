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

  static uint16_t localVariable = 100; // Declare as static to persist its value

  etimer_set(&et, CLOCK_SECOND * 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

  printf("A: ATTENTION: localVariable = %u", localVariable);
  printf(" (address=%p)\n", &localVariable); // Use %p for pointer addresses

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
  static uint16_t inc = 0;
  static struct etimer et;

  while (1)
  {
    // Perform a small chunk of computation
    for (int i = 0; i < 10000; i++)
    {
      if (inc % 10000 == 0)
      {
        printf("C: i %u\n", inc);
      }
      inc++;
    }

    // Yield control to other processes and wait for a short period
    etimer_set(&et, CLOCK_SECOND / 90); // Short delay to allow others to run
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
 *
 * What value does it have when you run the program a) before the button sensor event and b) after ? why is it like this? explain.
 *  Before the button sensor event:
 *    The situation of the before code When the process A_PROCESS initializes, it sets localVariable = 100 and prints its value 10 seconds after the node boots. 
 *     Therefore, before pressing the button, the value of localVariable is 100. The memory address where it is stored is also displayed. 
 *  After the button sensor event:
 *  After that, when the user presses the button, the process prints the value of localVariable. However, since localVariable is declared as a local variable inside the thread function, its lifetime ends when the function's execution context changes.
 *  By the time you press the button, the address might hold a garbage value or data overwritten by other processes.
 * What do you have to do in order to make sure that the value of the variable "localVariable" is
 * always 100?
 *  Answer:
 * To ensure the value of localVariable is always 100, make it a static or global variable in order to stabilize the value of 100 and because of this will retain its value throughout the program's lifetime, regardless of context switching.

 *   
 *
 * b) Protothread C is a computationally intensive task. It is already defined in the code above but not yet used.
 *
 * Let it start at boot time. Observe what happens.
 * 
 *
 * Protothread C eats up all CPU. Since it never returns control to the event handler, it monopolizes the CPU.
 *
 * As a programmer, you can solve such tasks in separate protothreads, while still being able to serve other protothreads.
 *
 * Find a solution to let C use the CPU while A and B are still being served. Check your final code with a Cooja simulator.
 *{
  PROCESS_BEGIN();
  static uint16_t inc = 0;
  static struct etimer et;

  while (1)
  {
    // Perform a small chunk of computation
    for (int i = 0; i < 10000; i++)
    {
      if (inc % 10000 == 0)
      {
        printf("C: i %u\n", inc);
      }
      inc++;
    }

    // Yield control to other processes and wait for a short period
    etimer_set(&et, CLOCK_SECOND / 90); // Short delay to allow others to run
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }


  PROCESS_END();
}

This is the modified version of process_thread C. An event timer introduces short delays (CLOCK_SECOND / 90) after processing each chunk, ensuring other protothreads get CPU time.
That being said, this will still serve other protothreads.
 */
