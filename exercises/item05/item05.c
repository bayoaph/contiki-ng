/* Philipp Hurni, University of Bern, December 2013
 * Exercises for sensor networks lecture
 */

#include "contiki.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h"   /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
#include "lib/random.h"
#include "dev/button-sensor.h" /* for the button sensor */

/*---------------------------------------------------------------------------*/
PROCESS(A_PROCESS, "A");
PROCESS(B_PROCESS, "B");
PROCESS(C_PROCESS, "C");
AUTOSTART_PROCESSES(&A_PROCESS, &B_PROCESS, &C_PROCESS);
/*---------------------------------------------------------------------------*/

/* Process A: Prints the value of a static local variable after 10s and whenever the button is pressed */
PROCESS_THREAD(A_PROCESS, ev, data)
{
  PROCESS_BEGIN();

  static struct etimer et;
  static uint16_t localVariable = 100; /* static ensures value remains consistent */

  etimer_set(&et, CLOCK_SECOND * 10);
PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));


  printf("A: ATTENTION: localVariable = %u (address=%p)\n", localVariable, (void *)&localVariable);

  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    /* localVariable still prints as 100 due to being static */
    printf("A: ATTENTION: localVariable = %u (address=%p)\n", localVariable, (void *)&localVariable);
  }

  SENSORS_DEACTIVATE(button_sensor);
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

/* Process B: Prints random numbers periodically */
PROCESS_THREAD(B_PROCESS, ev, data)
{
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

/* Process C: Increments a counter continuously but uses PROCESS_PAUSE() to yield CPU time */
PROCESS_THREAD(C_PROCESS, ev, data)
{
  PROCESS_BEGIN();

  static uint16_t inc;
  while (1) {
    if (inc % 10000 == 0) {
      printf("C: i %u\n", inc);
    }
    inc++;

    /* Yield control so other processes (A and B) can run */
    PROCESS_PAUSE();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

/*
 * Exercise 5 (to be solved with ILIAS):
 *
 * (a) In Process A, a variable "localVariable" is declared and printed out 10s after
 * the node boots with the value 100.
 * Node B keeps looping and printing random numbers.
 * When you press the "user" button, Process A again prints the value of "localVariable".
 *
 * Q: What value does it have when you run the program:
 *    a) before the button sensor event?
 *    b) after pressing the button?
 *    Why is it like this? Explain.
 *
 * A:
 * Before pressing the button, "localVariable" prints as 100, which is its initialized value.
 * After pressing the button, if "localVariable" is a normal local variable (non-static),
 * its value may not remain stable. Due to how protothreads work, local variables are not
 * guaranteed to persist across blocking calls and context switches. Therefore, it might
 * print an unexpected or corrupted value.
 *
 * To ensure "localVariable" always remains 100, declare it as 'static':
 * static uint16_t localVariable = 100;
 * By making it static, its value persists across yields and remains consistent both before
 * and after pressing the button.
 *
 * (b) Protothread C is a computationally intensive task. If started at boot time, it
 * monopolizes the CPU and prevents A and B from running properly.
 *
 * Why does this happen?
 * Protothreads cooperate by yielding control to each other. If Protothread C never yields,
 * it never gives the scheduler a chance to run A or B, thus "eating up" all CPU time.
 *
 * How to solve this?
 * Insert a yielding call, such as PROCESS_PAUSE(), in C’s loop:
 *
 * while(1) {
 *   // do some computation
 *   PROCESS_PAUSE(); // this yields back to the scheduler
 * }
 *
 * By calling PROCESS_PAUSE(), C allows other protothreads (A and B) to run as well.
 * This ensures that all processes are served fairly by the CPU.
 */

