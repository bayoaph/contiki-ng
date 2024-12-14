
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

// For simulation (Using sky mote):
#include <stdio.h> /* For printf() */
#include "dev/button-sensor.h" /* for the button sensor */
#include "dev/sht11/sht11-sensor.h"  /* for the temperature sensor */
#include "sys/node-id.h"  /* get the variable node_id that holds the own node id */
#include "dev/leds.h"

/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
/*---------------------------------------------------------------------------*/

void print_temperature_int_to_float(uint16_t temp)
{
  printf("%u.%uC\n", temp / 10, temp % 10);  // SHT11 reports in tenths of degrees 
}

PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);
  SENSORS_ACTIVATE(sht11_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_GREEN);

    printf("Temperature: ");
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));

    leds_off(LEDS_GREEN);
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

 My Answer:
 To run the program in the simulation, I change the zoul sensor to sht11-sensor.h to run the program using a sky mote 
 (as shown in the my simulation section below). Once the program is compiled and runned, the LED turns on everytime the 
 button is pressed. Additionally, the temperature sensor reads and prints the temperature in Celsius.
 */

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed


  My Answer:
  In here, everytime the button is pressed, the LED is turned on, then text "button pressed" is printed 
  before the LED is turned off.

  Modified Process thread:
  PROCESS_THREAD(button_press_process, ev, data)
  {
    PROCESS_BEGIN();

    SENSORS_ACTIVATE(button_sensor);

    while (1)
    {
      PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

      // Blink the LED
      leds_on(LEDS_YELLOW);
      printf("button pressed\n");
      leds_off(LEDS_YELLOW);
    }

    PROCESS_END();
  }

  
 */

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 * 


My Answer:
In here, everytime the button is pressed, the LED is turned on, then text "button pressed" is printed, 
and the temperature is read by the sensor then printed its value in Celsius through the  print_temperature_int_to_float()
before the LED is turned off.


Using zoul sensor
Modified program:
#include <stdio.h> // For printf() 
#include <string.h>
#include "dev/button-sensor.h"  //for the button sensor 
#include "dev/zoul-sensors.h"  //for the temperature sensor 
#include "node-id.h"            //get the variable node_id that holds the own node id 
#include "dev/leds.h"

-----------------------------------------------------------
PROCESS(button_press_process, "Button Press Process");
-----------------------------------------------------------
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

    // Turn on LED and print button press message
    leds_on(LEDS_YELLOW);
    printf("button pressed\n");

    // Print the temperature
    printf("Temperature: ");
    SENSORS_ACTIVATE(cc2538_temp_sensor);
    print_temperature_int_to_float(cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED));
    SENSORS_DEACTIVATE(cc2538_temp_sensor);

    leds_off(LEDS_YELLOW); // Turn off LED
  }

  PROCESS_END();
}
----------------------------------------------------
AUTOSTART_PROCESSES(&button_press_process);
-----------------------------------------------------


Using sky mote
Modified Program:
#include <stdio.h>                      //For printf() 
#include "dev/button-sensor.h"         // for the button sensor 
#include "dev/sht11/sht11-sensor.h"   // for the temperature sensor 
#include "sys/node-id.h"              // get the variable node_id that holds the own node id 
#include "dev/leds.h"

---------------------------------------------------------------------------
PROCESS(button_press_process, "Button Press Process");
---------------------------------------------------------------------------

void print_temperature_int_to_float(uint16_t temp)
{
  printf("%u.%uC\n", temp / 10, temp % 10);  // SHT11 reports in tenths of degrees
}

PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);
  SENSORS_ACTIVATE(sht11_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_GREEN);
    printf("button pressed\n");
    printf("Temperature: ");
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));

    leds_off(LEDS_GREEN);
  }

  SENSORS_DEACTIVATE(sht11_sensor);
  SENSORS_DEACTIVATE(button_sensor);

  PROCESS_END();
}

---------------------------------------------------------------------------
AUTOSTART_PROCESSES(&button_press_process);
---------------------------------------------------------------------------



*/


// original code:
// #include <stdio.h> /* For printf() */
// #include <string.h>
// #include "dev/button-sensor.h" /* for the button sensor */
// #include "dev/zoul-sensors.h"  /* for the temperature sensor */
// #include "sys/node-id.h"  /* get the variable node_id that holds the own node id */
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

//     leds_on(LEDS_YELLOW);
//     //printf("button pressed\n");  // For Exercise 2b

//     printf("Temperature: ");
//     SENSORS_ACTIVATE(cc2538_temp_sensor);
//     print_temperature_int_to_float(cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED));
//     SENSORS_DEACTIVATE(cc2538_temp_sensor);

//     leds_off(LEDS_YELLOW);
//     leds_off(LEDS_YELLOW);
//   }

//   PROCESS_END();
// }

// /*---------------------------------------------------------------------------*/
// AUTOSTART_PROCESSES(&button_press_process);
// /*---------------------------------------------------------------------------*/
