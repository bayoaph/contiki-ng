
/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */
#include "contiki.h"
#include "dev/button-sensor.h"  /* for the button sensor */
#include "dev/sht25.h"  /* for the SHT25 temperature sensor */
#include "dev/leds.h"
#include <stdio.h>  /* For printf() */

/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
/*---------------------------------------------------------------------------*/

// Helper function to display temperature in a readable format
static void print_temp_in_celsius(int16_t temp_in_tenths)
{
  printf("Temperature: %d.%d°C\n", temp_in_tenths / 10, temp_in_tenths % 10);
}

// Helper function to manage LED blinking
static void blink_led(uint8_t led_color, clock_time_t duration)
{
  leds_on(led_color);
  clock_wait(duration);  // Wait for the specified duration
  leds_off(led_color);
}

// Function to handle the temperature sensor reading
static void read_and_print_temperature(void)
{
  printf("Reading temperature...\n");
  
  SENSORS_ACTIVATE(sht25);
  int16_t temp_value = sht25.value(SHT25_VAL_TEMP);
  if (temp_value == -1)
  {
    printf("Error: Unable to read temperature.\n");
  }
  else
  {
    print_temp_in_celsius(temp_value);  // Print the formatted temperature
  }
  SENSORS_DEACTIVATE(sht25);
}

// Event-driven button press handling function
PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  // Activate the button sensor for event-based detection
  SENSORS_ACTIVATE(button_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor); // Wait for button press event

    printf("Button pressed\n");
    
    // Blink LED and read temperature
    blink_led(LEDS_GREEN, CLOCK_SECOND);  // Blink the LED for 1 second (color could be changed)
    read_and_print_temperature();  // Read and print temperature from sensor

  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface
 */

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 */

/*
 * Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */