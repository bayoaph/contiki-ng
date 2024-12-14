/* Philipp Hurni, University of Bern, December 2013     */
/* Exercises for sensor networks lecture                */

#include "contiki.h"
#include <stdio.h>
#include <string.h>
#include "node-id.h" 
#include "dev/leds.h"
#include "lib/random.h"
#include "dev/button-sensor.h" /* for the button sensor */

PROCESS(A_PROCESS, "A");


PROCESS_THREAD(A_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer et;

  etimer_set(&et, CLOCK_SECOND * 10);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  static const uint16_t localVariable = 100;
  printf("A: ATTENTION: localVariable = %u", localVariable);
  printf(" (address=%p)\n", (void*)&localVariable);


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


PROCESS(B_PROCESS, "B");


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


PROCESS(C_PROCESS, "C");



PROCESS_THREAD(C_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer et;
  static uint16_t inc;

  etimer_set(&et, 5 * CLOCK_SECOND);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    
    // Print the counter value every 5 seconds
    printf("C: i %u\n", inc);
    inc++;
    
    etimer_reset(&et);

    PROCESS_PAUSE(); 
  }

  PROCESS_END();
}
AUTOSTART_PROCESSES(&A_PROCESS, &B_PROCESS, &C_PROCESS); // autostart processes

/*
  *USE SKY MOTE for button sensor
*/
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
 answer
  *a) 100, as it is initialized as the constant value in process A
  *b) 100, as it is a local variable that was initialized once when process A starts, thus it does not change.
 
 * What do you have to do in order to make sure that the value of the variable "localVariable" is
 * always 100?
 answer
  *declare it as a constant const

 * b) Protothread C is a computationally intensive task. It is already defined in the code above but not yet used.
 * Let it start at boot time. Observe what happens.
 * Protothread C eats up all CPU. Since it never returns control to the event handler, it monopolizes the CPU.
 * As a programmer, you can solve such tasks in separate protothreads, while still being able to serve other protothreads.
 * Find a solution to let C use the CPU while A and B are still being served. Check your final code with a Cooja simulator.
 answer
  *introduce a timer that periodically pause process C that allows the scheduler to give control to other processes.
  PROCESS_THREAD(B_PROCESS, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer et;
  while (1)
  {
    etimer_set(&et, CLOCK_SECOND / 2);  // Set the timer to expire every 0.5 seconds
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));  // Wait until the timer expires
    uint16_t randomNum = random_rand();  // Generate a random number
    printf("B: %u (address=%p)\n", randomNum, &randomNum);  // Print the random number and its address
  }
  PROCESS_END();
}
 */