/* Philipp Hurni, University of Bern, December 2013     */
/* Exercises for sensor networks lecture                */

#include "contiki.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h"
#include "dev/leds.h"
#include "lib/random.h"
#include "dev/button-sensor.h"

/*---------------------------------------------------------------------------*/
PROCESS(A_PROCESS, "A");
PROCESS(B_PROCESS, "B");
PROCESS(C_PROCESS, "C");
AUTOSTART_PROCESSES(&A_PROCESS, &B_PROCESS, &C_PROCESS);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(A_PROCESS, ev, data) {
  PROCESS_BEGIN();
  static struct etimer et;

  etimer_set(&et, CLOCK_SECOND * 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  static uint16_t localVariable = 100;
  printf("A: ATTENTION: localVariable = %u", localVariable);
  printf(" (address=%p)\n", (void *)&localVariable);

  SENSORS_ACTIVATE(button_sensor);
  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    printf("A: ATTENTION: localVariable = %u", localVariable);
    printf(" (address=%p)\n", (void *)&localVariable);
  }
  SENSORS_DEACTIVATE(button_sensor);
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(B_PROCESS, ev, data) {
  PROCESS_BEGIN();
  static struct etimer et;
  while (1) {
    etimer_set(&et, CLOCK_SECOND / 2);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    uint16_t randomNum = random_rand();
    printf("B: %u (address=%p)\n", randomNum, (void *)&randomNum);
  }
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(C_PROCESS, ev, data) {
  PROCESS_BEGIN();
  static struct etimer et;
  static uint16_t inc;
  while (1) {
    if (inc % 10000 == 0) {
      printf("C: i %u\n", inc);
    }
    inc++;
    PROCESS_PAUSE();

    // Introduce a delay to allow other protothreads to execute
    etimer_set(&et, CLOCK_SECOND / 10); // Adjust frequency of yielding
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  PROCESS_END();
}

/* Exercise 5 (to be solved with ILIAS):
 *
 * a) In the process A, a variable "localVariable" is declared and printed out 10s after
 * the node boots with the value 100.
 *k
 * Node B keeps looping and printing random numbers
 *
 * When you press the "user" button, the process A again prints the value of the "localVariable"
 *
 * What value does it have when you run the program a) before the button sensor event and b) after ? why is it like this? explain.
 * 
 * a. The value that the "localVariable" have is 100 before the button sensor event. This is because "localVariable is declared as 
 * static variable where its value remains the same."
 * 
 * b. After the button is clicked, its value remains the same as 100. As mentioned on my previous answer, the "localVariable" is 
 * declared as a static. The value stored in memory does not change hence its value is still 100.
 * 
 * c. Static variables in C are stored in the data segment no in memory. They are only initialized once and retain their value 
 * for the lifetime of the program regardless of how many times the function of process is invoked.
 *
 * What do you have to do in order to make sure that the value of the variable "localVariable" is
 * always 100?
 * 
 * To ensure that the value of the variable "localVariable" is always 100, we need to ensure that it is not modified in the code.
 * If there are any cases where it will be modified, ensure that its value is reinitialize back to 100.
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