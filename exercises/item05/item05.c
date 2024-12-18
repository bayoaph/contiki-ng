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
  
  /* 
   * Declare localVariable as static to ensure it retains its value and does not get lost after function returns.
   * This ensures that the value printed after pressing the button remains 100.
   */
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
    /* Use %p to print addresses in a portable manner */
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

    /* 
     * Use PROCESS_PAUSE() to yield control to other processes, ensuring that 
     * A and B are still served and not blocked by C.
     */
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
 * What value does it have when you run the program 
 * a) before the button sensor event and 
 * b) after?
 * Why is it like this? Explain.
 *
 * Answer:
 * - Before the button sensor event, `localVariable` is printed as `100`, since it was initialized that way.
 * - After the button press, if `localVariable` were not static, its value might become corrupted or undefined,
 *   because it would not be retained in memory. By making it `static`, the variable remains in memory and
 *   keeps the value 100.
 *
 * What do you have to do in order to make sure that the value of the variable "localVariable" is
 * always 100?
 *
 * Answer:
 * - Declare `localVariable` as `static` so it retains its value across function calls and remains at the same 
 *   memory address.
 *
 * b) Protothread C is a computationally intensive task. 
 * Let it start at boot time and observe what happens.
 *
 * Initially, if C never yields, it monopolizes the CPU and prevents A and B from running properly.
 *
 * To solve this, add a `PROCESS_PAUSE()` in the C loop. This releases CPU control so that A and B can 
 * continue to execute, enabling all three processes to run fairly.
 *
 */
