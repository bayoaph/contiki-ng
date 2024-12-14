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

    // Introduce a delay to yield CPU and allow other protothreads to execute
    etimer_set(&et, CLOCK_SECOND / 10); // Allow other protothreads to execute periodically
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  PROCESS_END();
}

/* Exercise 5 (to be solved with ILIAS):
 *
 * a) The value of the "localVariable" is 100 before the button press because it is initialized to 100 and its value 
 * does not change until the button press.
 * 
 * b) After the button is pressed, the value remains 100, because "localVariable" is declared as static. It retains its 
 * value for the lifetime of the program, unless explicitly changed.
 * 
 * c) Static variables in C are stored in the data segment and retain their value throughout the program execution, even 
 * across multiple function calls.
 *
 * To ensure the value of "localVariable" is always 100, you need to avoid any modification to this variable in the code.
 * If needed, reinitialize it to 100 after each change or at the start of each event.
 *
 * b) Protothread C was computationally intensive and previously monopolized the CPU. By introducing `PROCESS_PAUSE()` 
 * and periodically yielding with `etimer`, it allows other protothreads to execute.
 *
 */
