
/* Philipp Hurni, University of Bern, December 2013*/

#include <stdio.h> 
#include <string.h>
#include "dev/button-sensor.h" 
#include "node-id.h"          
#include "dev/leds.h"
#include <stdlib.h>      

PROCESS(button_press_process, "Button Press Process");

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

    leds_on(LEDS_YELLOW);
    printf("button pressed\n");
    clock_wait(CLOCK_SECOND / 4); 
    leds_off(LEDS_YELLOW);
    clock_wait(CLOCK_SECOND / 4);

    printf("Temperature: ");
    uint16_t random_temp = (rand() % 40000);
    print_temperature_int_to_float(random_temp);
  }

  PROCESS_END();
}

AUTOSTART_PROCESSES(&button_press_process);

/*
  *I just generated a random temperature since zoul sensor wont work. USE SKY MOTE. 
*/
/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface
    When the button is pressed, the green LED briefly lights up and turns off. The serial interface then printed "button pressed" and the random temperature were generated.
 */

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 Changes to apply
    leds_on(LEDS_YELLOW);
    printf("button pressed\n");
    clock_wait(CLOCK_SECOND / 4); 
    leds_off(LEDS_YELLOW);
    clock_wait(CLOCK_SECOND / 4);

    printf("Temperature: ");
    uint16_t random_temp = (rand() % 40000);
    print_temperature_int_to_float(random_temp);
  
 */

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
  Changes to apply
    printf("Temperature: ");
    printf("Temperature: ");
    uint16_t random_temp = (rand() % 40000);
    print_temperature_int_to_float(random_temp);
 */