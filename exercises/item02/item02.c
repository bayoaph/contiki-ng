
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/sht11/sht11-sensor.h"  /* for the temperature sensor */
#include "dev/button-sensor.h" /* for the button sensor */

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
  static struct etimer timer_blink;

  SENSORS_ACTIVATE(button_sensor);


  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_GREEN);

    printf("Button pressed \n");

    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);

    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));
    leds_off(LEDS_GREEN);
  }

  PROCESS_END();  
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface

Answer:
Instead of the zoul-sensors, I have used the sht11 sensors as it is th one available in my device. 
Everytime the button is clicked, it prints an output of the temperature of  6.306 C
 */

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 

 ANSWER:
 In the process of button_press_process, the led used was GREEN as cooja has no BLUE. Additionally, a timer was used to make the leds blink:

 PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_blink;

  SENSORS_ACTIVATE(button_sensor);


  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_GREEN);

    printf("Button pressed \n");

    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));
    leds_off(LEDS_GREEN);
  }

  PROCESS_END();  
}

/
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 * 
 * ANSWER:
 * Aside from the button pressing and the leds, the temperature was also readded. The sht11 sensor was used as the temperature sensor and was activated when the button is pressed.
 *  sht11_sensor.value(SHT11_SENSOR_TEMP) was used to get the value of the temp sensor.
 * 
 * PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer_blink;

  SENSORS_ACTIVATE(button_sensor);


  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_GREEN);

    printf("Button pressed \n");

    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);

    etimer_set(&timer_blink, CLOCK_SECOND * 1);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_blink));
    leds_off(LEDS_GREEN);
  }

  PROCESS_END();  
}

 */