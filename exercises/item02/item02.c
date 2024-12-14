
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
#include "dev/sht25.h" /* for the temperature sensor */
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

  static struct etimer blink;

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_RED);
    printf("button pressed\n");

    etimer_set(&blink, 0.5 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&blink));
    leds_off(LEDS_RED);

    printf("Temperature: ");
    SENSORS_ACTIVATE(sht25);
    print_temperature_int_to_float(sht25.value(SHT25_VAL_TEMP));
    SENSORS_DEACTIVATE(sht25);

    leds_off(LEDS_RED);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/



/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface


The initial code fails to compile so I had to change the zoul sensor to sht25 and simulate using Z1 mote. 
I also modified "cc2538_temp_sensor" and "CC2538_SENSORS_VALUE_TYPE_CONVERTED" to "sht25" and "SHT25_VAL_TEMP",
respectively to align with the changing of the library.

The code below is the modified item02.c that compiles correctly using Z1 mote:

#include <stdio.h> 
#include <string.h>
#include "dev/button-sensor.h" 
#include "dev/sht25.h" 
#include "node-id.h"         
#include "dev/leds.h"

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

    leds_on(LEDS_RED);

    printf("Temperature: ");
    SENSORS_ACTIVATE(sht25);
    print_temperature_int_to_float(sht25.value(SHT25_VAL_TEMP));
    SENSORS_DEACTIVATE(sht25);

    leds_off(LEDS_RED);
  }

  PROCESS_END();
}

AUTOSTART_PROCESSES(&button_press_process);

Observation: After compilation, the LED blinks once. However, after the initial blink, 
the LED no longer blinks on subsequent button presses, although the temperature continues
to be printed on the serial interface each time the button is clicked.

*/



/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed

For 2b, I declared a static struct etimer named "blink" that will be set it to a duration of 0.5 seconds 
to simulate the LED blinking. The LED is turned on, waits for 0.5 seconds, and then turns off.

I also added the printf("button pressed\n") statement inside the while loop to ensure that "button pressed" 
is printed every time the button is clicked.

The code below shows the modified process thread.

PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);

  static struct etimer blink;

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_RED);
    printf("button pressed\n");

    etimer_set(&blink, 0.5 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&blink));
    leds_off(LEDS_RED);

    printf("Temperature: ");
    SENSORS_ACTIVATE(sht25);
    print_temperature_int_to_float(sht25.value(SHT25_VAL_TEMP));
    SENSORS_DEACTIVATE(sht25);

    leds_off(LEDS_RED);
  }

  PROCESS_END();
}

*/



/*

 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().

Since the code for 2b already performs this action, the implementation for Exercise 2c is essentially the same. 
Thus, there is no need for any further changes, as the expected behavior of reading and printing the temperature 
when the button is pressed has already been achieved in the previous exercise.

*/