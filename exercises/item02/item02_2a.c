/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */
/* Modified by Kent Dico, 2024. Target=z1 */

#include "contiki.h"
#include "dev/button-sensor.h"  /* for the button sensor */
#include "dev/sht25.h"          /* Used the SHT25 temperature sensor instead of zoul sensor */
#include "dev/leds.h"           /* For controlling LEDs */
#include <stdio.h>              

/*---------------------------------------------------------------------------*/
// Function to print the temperature in a floating-point style format
void print_temperature_int_to_float(int16_t temp)
{
  printf("%d.%d°C\n", temp / 10, temp % 10);  // Format temperature in tenths of a degree
}

/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  // Activate the button sensor
  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    // Wait for a button press event
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    // Turn on the blue LED
    leds_on(LEDS_BLUE);

    // Print temperature
    printf("Temperature: ");
    SENSORS_ACTIVATE(sht25);  // Activate the SHT25 sensor
    int16_t temperature = sht25.value(SHT25_VAL_TEMP);
    if (temperature != -1) {
      print_temperature_int_to_float(temperature);  // Format and print the temperature
    } else {
      printf("Error: Unable to read temperature.\n");
    }
    SENSORS_DEACTIVATE(sht25);  // Deactivate the sensor

    // Turn off the blue LED
    leds_off(LEDS_BLUE);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2a: compile and run the program. press the button, observe what it does with
 * the serial interface
 * Observation:
 * - Button press triggers temperature reading.
 * - Temperature is fetched from the SHT25 sensor and displayed as Temperature: X.Y°C on the serial interface.
 * - The blue LED turns on but does not blink.
 */

/* Changes:
 * - Used z1 target instead of zoul.
 * - Changed the temperature sensor to SHT25.
 */

