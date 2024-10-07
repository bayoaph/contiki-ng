
/* Based on exercises by Philipp Hurni, University of Bern, December 2013     */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
#include "dev/zoul-sensors.h"
#include "node-id.h" /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);
  static uint16_t temp;

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_BLUE);

    printf("Temperature: ");
    SENSORS_ACTIVATE(cc2538_temp_sensor);
    temp = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
    printf("%u.%03uC\n", (unsigned)temp / 1000, (unsigned)temp % 1000);
    SENSORS_DEACTIVATE(cc2538_temp_sensor);

    leds_off(LEDS_BLUE);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface
 */

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 */

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed.
 *
 */