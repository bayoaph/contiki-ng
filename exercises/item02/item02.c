
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include "dev/button-sensor.h" /* For the button sensor */
#include "dev/sht11-sensor.h" /* For the temperature sensor */
#include "node-id.h" /* For the variable node_id */
#include "dev/leds.h" /* For LED control */
/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
/*---------------------------------------------------------------------------*/
// exercise 2c
void print_temperature_int_to_float(uint16_t temp)
{
  printf("%u.%uC\n", temp / 10, temp % 10);
}

PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  // Activate the button sensor
  SENSORS_ACTIVATE(button_sensor);

  while (1)
  {
    // Wait until the button is pressed
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_RED); // Turn on the red LED to indicate activity

    // Activate the temperature sensor
    SENSORS_ACTIVATE(sht11_sensor);
    uint16_t temp = sht11_sensor.value(SHT11_SENSOR_TEMP);
    // exercise 2b
    printf("button pressed\n");
    printf("Temperature: ");
    print_temperature_int_to_float(temp);
    SENSORS_DEACTIVATE(sht11_sensor); // Deactivate the sensor after reading

    leds_off(LEDS_RED); // Turn off the red LED
  }

  PROCESS_END();
}


/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface

 Before running the programming code, I change the sensors, zoul-sensor to sht11-sensor, and cc2538 to sht11 using sky mote.
 After compiling, the code simulates the LED light turns on when you click the button, and print the Temperature with temperature
degree in Celcius.


 */

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 In modifying the code just I added printf function.
 After modifying the code, the simulator printed "button presses".
 PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_RED);
printf("button pressed\n");
...

    leds_off(LEDS_RED);
  }

  PROCESS_END();
}
 */

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 * Since im using sky mote, the modified code using print_temperature_int_to_float()
 * 
 */
// #include <stdio.h> /* For printf() */
// #include "dev/button-sensor.h" /* For the button sensor */
// #include "dev/sht11-sensor.h" /* For the temperature sensor */
// #include "node-id.h" /* For the variable node_id */
// #include "dev/leds.h" /* For LED control */
// /*---------------------------------------------------------------------------*/
// PROCESS(button_press_process, "Button Press Process");
// /*---------------------------------------------------------------------------*/
// // exercise 2c
// void print_temperature_int_to_float(uint16_t temp)
// {
//   printf("%u.%uC\n", temp / 10, temp % 10);
// }

// PROCESS_THREAD(button_press_process, ev, data)
// {
//   PROCESS_BEGIN();

//   // Activate the button sensor
//   SENSORS_ACTIVATE(button_sensor);

//   while (1)
//   {
//     // Wait until the button is pressed
//     PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

//     leds_on(LEDS_RED); // Turn on the red LED to indicate activity

//     // Activate the temperature sensor
//     SENSORS_ACTIVATE(sht11_sensor);
//     uint16_t temp = sht11_sensor.value(SHT11_SENSOR_TEMP);
//     // exercise 2b
//     printf("button pressed\n");
//     printf("Temperature: ");
//     print_temperature_int_to_float(temp);
//     SENSORS_DEACTIVATE(sht11_sensor); // Deactivate the sensor after reading

//     leds_off(LEDS_RED); // Turn off the red LED
//   }

//   PROCESS_END();
// }
