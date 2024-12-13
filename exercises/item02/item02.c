
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
/* #include "dev/zoul-sensors.h"  // for the temperature sensor 
 * This part was changed since there were errors when using this
 */
#include "arch/dev/sensor/sht11/sht11-sensor.h"  /* for the temperature sensor */
#include "node-id.h"           /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
/*---------------------------------------------------------------------------*/

void print_temperature_int_to_float(uint16_t temp)
{
  printf("%u.%uC\n", temp / 1000, temp % 1000);
}

PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);

  static struct etimer timer_blink;

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    // Exercise 2b: Blink led and print text
    printf("button pressed");

    leds_on(LEDS_GREEN);
    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));

    // Exercise 2c: Read temp from sensor
    printf("Temperature: ");
    SENSORS_ACTIVATE(cc2538_temp_sensor);
    print_temperature_int_to_float(cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED));
    SENSORS_DEACTIVATE(cc2538_temp_sensor);

    leds_off(LEDS_GREEN);
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
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */