
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */

#include <stdio.h> /* For printf() */
#include "dev/button-sensor.h" /* For the button sensor */
#include "dev/sht25.h" /* For the temperature sensor */
#include "dev/leds.h" /* For controlling LEDs */
#include "sys/etimer.h" /* For event timers */
#include "sys/process.h" /* For process control */
/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Helper function to print temperature in float format */
static void print_temperature(uint16_t temp)
{
  printf("Temperature: %u.%uC\n", temp / 1000, temp % 1000);
}

PROCESS_THREAD(button_press_process, ev, data)
{
  static struct etimer blink_timer;

  PROCESS_BEGIN();

  /* Activate sensors */
  SENSORS_ACTIVATE(button_sensor);
  SENSORS_ACTIVATE(sht25);

  while (1)
  {
    /* Wait for button press event */
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    /* Indicate button press */
    printf("Button pressed\n");
    leds_on(LEDS_GREEN);

    /* Blink the LED with a 250ms delay */
    etimer_set(&blink_timer, CLOCK_SECOND / 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&blink_timer));
    leds_off(LEDS_GREEN);

    /* Read and print temperature */
    print_temperature(sht25.value(SHT25_VAL_TEMP));
  }

  /* Deactivate sensors (cleanup if process ends) */
  SENSORS_DEACTIVATE(button_sensor);
  SENSORS_DEACTIVATE(sht25);

  PROCESS_END();
}


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