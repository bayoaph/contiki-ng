/* Philipp Hurni, University of Bern, December 2013     */
/* Modified for Zolertia Firefly - M. Cabilo, Apr 2019  */
/* Modified by Kent Dico, 2024. Target=z1 */

#include "contiki.h"
#include "dev/button-sensor.h"  /* for the button sensor */
#include "dev/sht25.h"          /* Used the SHT25 temperature sensor instead of zoul sensor */
#include "dev/leds.h"           /* For controlling LEDs */
#include <stdio.h>              
#include "sys/etimer.h"         /* For timer functionality */

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
  static struct etimer timer;
  static int i;
  PROCESS_BEGIN();

  // Activate the button sensor
  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    // Wait for a button press event
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    // Ensure LED blinking behavior works correctly
    for (i = 0; i < 3; i++) {
      leds_toggle(LEDS_YELLOW); // Using yellow LED for better visibility
      etimer_set(&timer, CLOCK_SECOND / 4); // 250 ms on/off
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    }
    leds_off(LEDS_YELLOW); // Ensure the LED is turned off after blinking

    // Print "button pressed"
    printf("Button pressed\n");

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
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/

/* Exercise 2c: read out the temperature from the temperature
 * sensor when the button is pressed. print the temperature to the serial interface by
 * passing the value read from the sensor to print_temperature_int_to_float().
 */

/* Observations for Exercise 2c:
 * - The temperature reading is already being done and printed in the format X.Y°C.
 * - Button press works fine and triggers temperature reading.
 * - LED still blinks three times (250ms on/off) during the button press.
 * - No additional changes were needed for temperature reading since it’s implemented.
 */

/* Changes for Exercise 2c:
 * - No new changes were needed; temperature reading with print_temperature_int_to_float() was already implemented.
 * - Retained all prior functionalities.
 */
