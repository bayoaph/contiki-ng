
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
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

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_RED);
    //printf("Button Pressed\n");
    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor); //activate sht11 instead of the dev/cc2538-temp-sensor.h
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP)); //use sht11 sensor as well
    SENSORS_DEACTIVATE(sht11_sensor);

    leds_off(LEDS_RED); //turn this to red to see
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface
 */
/********************************************************************************************************
I changed zoul to "arch/dev/sensor/sht11/sht11-sensor.h" and the dev/cc2538-temp-sensor.h to 
sht11_sensor for the temperature and changed the color to red because blue was no visible. 
Pressing the button lights the color and presents the temperature.

#include "arch/dev/sensor/sht11/sht11-sensor.h"  // for the temperature sensor 

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

    leds_on(LEDS_RED);
    printf("button pressed");
    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor); //activate sht11 instead of the dev/cc2538-temp-sensor.h
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP)); //use sht11 sensor as well
    SENSORS_DEACTIVATE(sht11_sensor);

    leds_off(LEDS_RED); //turn this to red to see
  }

  PROCESS_END();
}

*********************************************************************************************************/



/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 */

/**********************************************************************************************************
void print_temperature_int_to_float(uint16_t temp)
{
  //printf("%u.%uC\n", temp / 1000, temp % 1000);
}

PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_RED);
    printf("Button Pressed\n");
    //printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor); //activate sht11 instead of the dev/cc2538-temp-sensor.h
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP)); //use sht11 sensor as well
    SENSORS_DEACTIVATE(sht11_sensor);

    leds_off(LEDS_RED); //turn this to red to see
  }

  PROCESS_END();
}
*************************************************************************************************************/




/*************************************************************************************************************
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */
/*
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

    leds_on(LEDS_RED);
    //printf("Button Pressed\n");
    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor); //activate sht11 instead of the dev/cc2538-temp-sensor.h
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP)); //use sht11 sensor as well
    SENSORS_DEACTIVATE(sht11_sensor);

    leds_off(LEDS_RED); //turn this to red to see
  }

  PROCESS_END();
}
***************************************************************************************************************/