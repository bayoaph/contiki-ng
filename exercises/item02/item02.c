/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h>          /* For printf() */
#include "contiki.h"        /* Contiki core */
#include "dev/leds.h"       /* LED control */
#include "lib/random.h"     /* For simulated temperature values */

/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Simulated temperature function */
int get_simulated_temperature() {
  return 2000 + (random_rand() % 1000); // Simulates temperatures between 20.00°C and 29.99°C
}

/* Helper function to print temperature in a readable format */
void print_temperature(int temp_raw) {
  float temp = temp_raw / 100.0;
  printf("Temperature: %.2f°C\n", temp);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(button_press_process, ev, data)
{
  static struct etimer timer; // Timer for simulation and LED delay
  PROCESS_BEGIN();

  printf("Cooja Simulation: Press button to read temperature.\n");

  while (1) {
    /* Simulate button press at intervals (every 5 seconds) */
    etimer_set(&timer, CLOCK_SECOND * 5);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

    /* Simulate LED blink */
    leds_on(LEDS_RED);
    printf("LED ON: Button pressed!\n");

    /* Simulate temperature reading */
    int temp = get_simulated_temperature();
    print_temperature(temp);

    /* Use etimer for a 500 ms delay */
    etimer_set(&timer, CLOCK_SECOND / 2); // 500 ms delay
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

    leds_off(LEDS_RED);
    printf("LED OFF\n");
  }

  PROCESS_END();
}

/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface
 */

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 */

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */