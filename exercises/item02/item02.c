
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
// #include "dev/sht11/sht11-sensor.h" 
#include "../../main_contiki/arch/dev/sensor/sht11/sht11-sensor.h"
// #include "dev/sky-sensors.h"
// #include "dev/zoul-sensors.h"  /* for the temperature sensor */
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
    printf("button pressed\n");
    printf("Temperature: ");
    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);

    leds_off(LEDS_RED);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface
 */
 /**************************************************************************************************
  Compiling and running the program had a few setbacks. This forced me to use the sky mote. The code
  was also changed to accomodate for the change of mote type. Fortunately, this allowed me to
  successfully run the program. After clicking the button, "Temperature: 6.360C" is printed.
  Moreover, the LED (red) turns on and turns of, although you may not catch it since it blinks
  fast. You can observe it better if you slow down the speed to 0.1x or slower.

// #include <stdio.h> /* For printf() */
// #include <string.h>
// #include "dev/button-sensor.h" /* for the button sensor */
// // #include "dev/sht11/sht11-sensor.h" 
// #include "../../main_contiki/arch/dev/sensor/sht11/sht11-sensor.h"
// // #include "dev/sky-sensors.h"
// // #include "dev/zoul-sensors.h"  /* for the temperature sensor */
// #include "node-id.h"           /* get the variable node_id that holds the own node id */
// #include "dev/leds.h"
// /*---------------------------------------------------------------------------*/
// PROCESS(button_press_process, "Button Press Process");
// /*---------------------------------------------------------------------------*/

// void print_temperature_int_to_float(uint16_t temp)
// {
//   printf("%u.%uC\n", temp / 1000, temp % 1000);
// }

// PROCESS_THREAD(button_press_process, ev, data)
// {
//   PROCESS_BEGIN();

//   SENSORS_ACTIVATE(button_sensor);

//   while (1)
//   {
//     PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

//     leds_on(LEDS_RED);
//     printf("Temperature: ");
//     SENSORS_ACTIVATE(sht11_sensor);
//     print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
//     SENSORS_DEACTIVATE(sht11_sensor);

//     leds_off(LEDS_RED);
//   }

//   PROCESS_END();
// }

// /*---------------------------------------------------------------------------*/
// AUTOSTART_PROCESSES(&button_press_process);
// /*---------------------------------------------------------------------------*/

//  **************************************************************************************************/

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 */
  /**************************************************************************************************
  This alteration does not change many things for the program. On top the previous processes it
  does, it now also prints "button pressed" (and newline) after the LED is turned on and before
  the "Temperature: 6.360C" is printed.

//   #include <stdio.h> /* For printf() */
// #include <string.h>
// #include "dev/button-sensor.h" /* for the button sensor */
// // #include "dev/sht11/sht11-sensor.h" 
// #include "../../main_contiki/arch/dev/sensor/sht11/sht11-sensor.h"
// // #include "dev/sky-sensors.h"
// // #include "dev/zoul-sensors.h"  /* for the temperature sensor */
// #include "node-id.h"           /* get the variable node_id that holds the own node id */
// #include "dev/leds.h"
// /*---------------------------------------------------------------------------*/
// PROCESS(button_press_process, "Button Press Process");
// /*---------------------------------------------------------------------------*/

// void print_temperature_int_to_float(uint16_t temp)
// {
//   printf("%u.%uC\n", temp / 1000, temp % 1000);
// }

// PROCESS_THREAD(button_press_process, ev, data)
// {
//   PROCESS_BEGIN();

//   SENSORS_ACTIVATE(button_sensor);

//   while (1)
//   {
//     PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

//     leds_on(LEDS_RED);
//     printf("button pressed\n");
//     printf("Temperature: ");
//     SENSORS_ACTIVATE(sht11_sensor);
//     print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
//     SENSORS_DEACTIVATE(sht11_sensor);

//     leds_off(LEDS_RED);
//   }

//   PROCESS_END();
// }

// /*---------------------------------------------------------------------------*/
// AUTOSTART_PROCESSES(&button_press_process);
// /*---------------------------------------------------------------------------*/
//  **************************************************************************************************/

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */
  /**************************************************************************************************
  As this program has already been implemented, I only ran the same code from 2b. The temperature
  is printed.

  //   #include <stdio.h> /* For printf() */
// #include <string.h>
// #include "dev/button-sensor.h" /* for the button sensor */
// // #include "dev/sht11/sht11-sensor.h" 
// #include "../../main_contiki/arch/dev/sensor/sht11/sht11-sensor.h"
// // #include "dev/sky-sensors.h"
// // #include "dev/zoul-sensors.h"  /* for the temperature sensor */
// #include "node-id.h"           /* get the variable node_id that holds the own node id */
// #include "dev/leds.h"
// /*---------------------------------------------------------------------------*/
// PROCESS(button_press_process, "Button Press Process");
// /*---------------------------------------------------------------------------*/

// void print_temperature_int_to_float(uint16_t temp)
// {
//   printf("%u.%uC\n", temp / 1000, temp % 1000);
// }

// PROCESS_THREAD(button_press_process, ev, data)
// {
//   PROCESS_BEGIN();

//   SENSORS_ACTIVATE(button_sensor);

//   while (1)
//   {
//     PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

//     leds_on(LEDS_RED);
//     printf("button pressed\n");
//     printf("Temperature: ");
//     SENSORS_ACTIVATE(sht11_sensor);
//     print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
//     SENSORS_DEACTIVATE(sht11_sensor);

//     leds_off(LEDS_RED);
//   }

//   PROCESS_END();
// }

// /*---------------------------------------------------------------------------*/
// AUTOSTART_PROCESSES(&button_press_process);
// /*---------------------------------------------------------------------------*/
//  **************************************************************************************************/
//  **************************************************************************************************/