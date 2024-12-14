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
      leds_toggle(LEDS_YELLOW);
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

/* Exercise 2b: alter the program such that whenever the button is pressed, the led
 * blinks and the string "button pressed" is printed
 */

/* Observations for Exercise 2b:
 * - Blue LED doesn’t work. Switched to yellow.
 * - LED blinks three times (250ms on/off).
 * - Button press message "Button pressed" prints fine.
 * - Temperature reading and print works as expected.
 */

/* Changes:
 * - Used yellow LED since blue doesn’t light up.
 * - Made LED blink three times for visibility.
 * - Made sure LED turns off after blinking.
 * - Print "Button Pressed" on each button click
 * - Temperature print kept as is.
 */

