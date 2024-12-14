
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
/* #include "dev/zoul-sensors.h"  - for the temperature sensor */
#include "arch/dev/sensor/sht11/sht11-sensor.h" /* Replacement for the zoul-sensor temperature sensor */
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
  SENSORS_ACTIVATE(sht11_sensor);

  static struct etimer var1;

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    printf("button pressed\n");

    leds_on(LEDS_GREEN);
    etimer_set(&var1, 0.5 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&var1));
    leds_off(LEDS_GREEN);

    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);

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

/* Exercise 2a: Answer

  Due to the failure of compilation of the initial code, there are changes that are made to the code first:
    - the zoul sensors are replaced with sht11-sensor, these sensors are then simulated using sky mote.
    - to align the change of used library, the following are also replaced:
      - cc2538_temp_sensor                  -> sht11_sensor
      - CC2538_SENSORS_VALUE_TYPE_CONVERTED -> SHT11_SENSOR_TEMP

  After the successful compilation, the following were observed:
    - when a mote's button is pressed, the GREEN LED is lit up half a second and then it displays the temperature detected by the sensor

*/


/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
*/

/* Exercise 2b: Answer

  This line of code allows the program to display "button pressed/n":

    printf("button pressed\n");
  
  Adding it to the process that is executed whenever the button is pressed, we can display the string whenever that process is executed.

*/


/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
*/

/* Exercise 2c: Answer

  using this line of code:

    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));

  the temperature can now be printed during the print_temperature_int_to_float() function.

*/







