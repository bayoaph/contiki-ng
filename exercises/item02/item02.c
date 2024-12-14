
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
//#include "dev/zoul-sensors.h"  /* for the temperature sensor */
#include "dev/sensor/sht11/sht11-sensor.h" /* for the temperature sensor */
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

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    // Exercise 2b: Blink LED and print "button pressed"
    leds_on(LEDS_GREEN);
    printf("Button pressed\n");
    clock_wait(CLOCK_SECOND / 4); 
    leds_off(LEDS_GREEN);
    clock_wait(CLOCK_SECOND / 4);

    // printf("Temperature: ");
    // SENSORS_ACTIVATE(cc2538_temp_sensor);
    // print_temperature_int_to_float(cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED));
    // SENSORS_DEACTIVATE(cc2538_temp_sensor);


    // Exercise 2c: Read temperature from sensor and print
    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/*
  *The sht11 sensor is used since the zoul_sensor won't work. This is also ran in SKYMOTE
  *since the button sensor wont work in the COOJAMOTE.
*/
/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface
    When pressing the button, the green light briefly lights up then turns off. It is not noticeable at 
    1x Speed but at 0.01x you can see it. The temperature from the sht11 sensor also prints in the 
    serial interface
 */

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
    ADD THE CODE BELOW:
    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);
    leds_on(LEDS_GREEN);
    printf("Button pressed\n");
    clock_wait(CLOCK_SECOND / 4); 
    leds_off(LEDS_GREEN);
    clock_wait(CLOCK_SECOND / 4);
  
 */

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
    ADD THE CODE BELOW:
    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);
 */