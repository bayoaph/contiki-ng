/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
#include "dev/sht11/sht11-sensor.h" /* for the temperature sensor */
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

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

  

    printf("Button pressed\n");

    leds_on(LEDS_RED);
    clock_wait(CLOCK_SECOND / 4); // LED on for 250ms
    leds_off(LEDS_RED);
    clock_wait(CLOCK_SECOND / 4); // LED off for 250ms
  

    

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




/* Answer 2a
      I honestly do not figured out for the first try on why and how the original code does not work on my end. However, I figured out that the zoul is not used for the temperature sensor
      What I did was transfer the sht11_sensor from the arch-ops-dev to the code file and use sky moote instead of cooja mote to run the code.

      After altering the codes, I run the button interface and  the sky moote output shows with the Temperature : 6.36 degrees celcius. 
*/



/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed

 Modified version of the code:
while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

  

    printf("Button pressed\n");

    leds_on(LEDS_RED);
    clock_wait(CLOCK_SECOND / 4); // LED on for 250ms
    leds_off(LEDS_RED);
    clock_wait(CLOCK_SECOND / 4); // LED off for 250ms
  

    

    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);

   
  }

  After the code is altered, the moote output shows printing the "button pressed" and the temperature. The led also blinks and I used red led color.



 Answer 2b 

 */

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 * 
 */
