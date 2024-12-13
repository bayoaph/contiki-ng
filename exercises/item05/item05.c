/* Philipp Hurni, University of Bern, December 2013     */
/* Exercises for sensor networks lecture                */

#include "contiki.h"
#include <stdio.h>
#include <string.h>
#include "sys/node-id.h" /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
#include "lib/random.h"
#include "dev/button-sensor.h" /* for the button sensor */
/*---------------------------------------------------------------------------*/
PROCESS(A_PROCESS, "A");
PROCESS(B_PROCESS, "B");
PROCESS(C_PROCESS, "C");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(A_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer et;

  etimer_set(&et, CLOCK_SECOND * 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  static uint16_t localVariable = 100; // add const to make it read-only
  printf("A: ATTENTION: localVariable = %u", localVariable);  // Print value, not address
  printf(" (address=%p)\n", (void*)&localVariable);            // Print address using %p

  SENSORS_ACTIVATE(button_sensor);
  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    printf("A: ATTENTION: localVariable = %u", localVariable);  // Print value
    printf(" (address=%p)\n", (void*)&localVariable);            // Print address using %p
  }
  SENSORS_DEACTIVATE(button_sensor);
  PROCESS_END();
}

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
    printf("B: %u (address=%p)\n", randomNum, (void*)&randomNum);  // Print value and address
  }
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(C_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static uint16_t inc;
  static struct etimer et;

  while (1) {
    if (inc % 10000 == 0)
      printf("C: i %u\n", inc);
    inc++;
    // Temporarily give the processor to another protothread
    // PROCESS_PAUSE();
    // Add an etimer to yield CPU
    etimer_set(&et, CLOCK_SECOND / 100);  // Small delay to yield time
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
 * Answer:
 * Before the button sensor event, the localVariable is already initialized with the value 100. 
 * This value is printed 10 seconds after program start since etimer is set to 10 seconds. 
 * But after the button sensor event and once the button is pressed, the process prints the value
 * of the localVariable again. 
 *
 * What do you have to do in order to make sure that the value of the variable "localVariable" is
 * always 100?
 * Answer:
 * Since the localVariable is already declared as static, it is ensured to be 100 since this keyword
 * ensures that the variable is allocated once and retains its value across function calls or events
 * within the same process. But, to prevent it from accidental modification within the process, we 
 * can declare it as "const" to make it rad-only:
 * static const uint16_t localVariable = 100;
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
 * Answer:
 * Process C runs an intensive computation loop without yielding control. Since it doesn't give back
 * CPU time, it monopolizes the CPU and starves Process A and B, which causes Process A to fail to 
 * respond to the button sensor, and Process B to stop printing random numbers.
 * 
 * To allow Process A and B to function while Process C performs its task, we can modify Process C to
 * yield CPU time periodically by using PROCESS_PAUSE() or etimer to let other processes execute.
 * 
 * Modification:
 * PROCESS_THREAD(C_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static uint16_t inc;
  static struct etimer et;

  while (1) {
    if (inc % 10000 == 0)
      printf("C: i %u\n", inc);
    inc++;

    // Add an etimer to yield CPU
    etimer_set(&et, CLOCK_SECOND / 100);  // Small delay to yield time
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }

  PROCESS_END();
}

 * 
 */