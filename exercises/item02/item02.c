/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include "contiki.h"
#include "etimer.h"
#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
#include "dev/sht25.h"  /* for the SHT25 temperature sensor */
#include "node-id.h"           /* get the variable node_id that holds the own node id */
#include "dev/leds.h"

/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
/*---------------------------------------------------------------------------*/

void print_temperature_int_to_float(int16_t temp)
{
  // The SHT25 temperature reading is in tenths of degrees Celsius
  printf("%d.%dC\n", temp / 10, temp % 10);
}

PROCESS_THREAD(button_press_process, ev, data)
{
  static struct etimer off_timer;

  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    // Exercise 2b: LED turns on and print "button pressed" when the button is pressed
    leds_on(LEDS_GREEN);
    printf("Button pressed\n");

    // Set up the timer to turn off the LED after 2 seconds
    etimer_set(&off_timer, 2 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&off_timer));
    leds_off(LEDS_GREEN);

    // Exercise 2c: Read and print temperature when the button is pressed
    printf("Temperature: ");
    SENSORS_ACTIVATE(sht25);

    int16_t temp = sht25.value(SHT25_VAL_TEMP);
    if (temp == -1) {
      printf("Error reading temperature sensor\n");
    } else {
      print_temperature_int_to_float(temp);
    }

    SENSORS_DEACTIVATE(sht25);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2a: Compile and run the program. Press the button, observe what it does with
 * the serial interface
 */

/* Exercise 2b: Alter the program such that whenever the button is pressed, the LED
 * blinks and the string "button pressed" is printed
 */

/* Exercise 2c: Read out the temperature from the temperature
 * sensor when the button is pressed. Print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */
