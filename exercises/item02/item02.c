/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
#include "dev/sensor/sht11/sht11-sensor.h"  /* for the temperature sensor (SHT11) */
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

    leds_on(LEDS_GREEN);
    printf("button pressed\n");

    printf("Temperature: ");

    // Activate the SHT11 temperature sensor
    SENSORS_ACTIVATE(sht11_sensor);
    
    // Get the temperature value from the sensor and print it
    uint16_t temperature = sht11_sensor.value(SHT11_SENSOR_TEMP);
    print_temperature_int_to_float(temperature);
    
    // Deactivate the sensor
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

 // My answer: 
 // - when i click the button, the temperature is printed, and the BLUE led blinks

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 */
// My answer: 
// I added a print statement so that whenever a button is pressed, it will print "button pressed"
/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */

// My answer : 
// The program already reads the temperature from the sensor and prints it when the button is pressed.
// But changed the sensor so that it will not give error. 