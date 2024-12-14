
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
//#include "dev/zoul-sensors.h"  /* for the temperature sensor */
#include "dev/sht11/sht11-sensor.h"
#include "sys/node-id.h"           /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
/*---------------------------------------------------------------------------*/

void print_temperature_int_to_float(uint16_t temp)
{
  printf("%u.%uC\n", temp / 10, temp % 10);
}

PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);
  SENSORS_ACTIVATE(sht11_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

     // Blink the LED
    leds_on(LEDS_RED);            // Turn the LED on
    clock_delay(400);              // Short delay for LED blink
    leds_off(LEDS_RED);           // Turn the LED off

    // Print "button pressed" to the serial interface
    printf("button pressed\n");

    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);

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
 //ANSWER: Since the zoul sensor doesn't apparently work, i had to make use of sht11 sensor.
 //Also, instead of Cooja mote, we had to make use of sky mote.
 //After compiling and running, in the Mote Output, a temperature of 6.360C was printed

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 */
 //ANSWER:
 //Apparently, since the  blue led doesn't work, I used red led instead. I opened to window interface
 //in order to show the button and the led. After running the simulation and clicking the button,
 //the red led blinked and the text "button pressed" was displayed in the mote output as well as the 
 //corresponding temperature.
  // Blink the LED
  //  leds_on(LEDS_RED);            // Turn the LED on
  //  clock_delay(400);              // Short delay for LED blink
  //  leds_off(LEDS_RED);           // Turn the LED off

    // Print "button pressed" to the serial interface
  //  printf("button pressed\n");

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */
 //ANSWER:
 //When the button is pressed, a Temperature of 6.360C was printed, displayed in the Mote output