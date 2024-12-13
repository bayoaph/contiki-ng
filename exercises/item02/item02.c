
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
#include "arch/dev/sensor/sht11/sht11-sensor.h"
//#include "dev/zoul-sensors.h"  /* for the temperature sensor */
#include "sys/node-id.h"           /* get the variable node_id that holds the own node id */
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

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_RED);
    printf("button pressed");
    printf("Temperature: ");
    
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));

    leds_off(LEDS_RED);
  }

  SENSORS_DEACTIVATE(sht11_sensor);
  SENSORS_DEACTIVATE(button_sensor);


  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface
 */

// At first, when i compiled the program I am having compilation error. Instead of using ZOUL i opted to use SKY. I installed necessary
// packages that needs to be installed (msp430-gcc) and edited the MAKEFILE of item 2.
//When i run the program and pressed the button, the LED also lits up and the prints the temperature that was detected by the sensor.

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 */

// In the code that I have above, I inserted the code printf("button pressed"). This will print "button pressed", everytime i pressed
// the button of the mote.

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */

//In this exercise, when the button is pressed, the temperature is printed during the print_temperature_int_to_float() function.
//The printed temperature is in degrees Celcius.