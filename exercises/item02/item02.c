#include <stdio.h>
#include <stdint.h>
#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/sensor/sht11/sht11-sensor.h" /* for the temperature sensor */
#include "dev/leds.h"

/* Declare a process. */
PROCESS(button_press_process, "Button Press Process");
AUTOSTART_PROCESSES(&button_press_process);

/* Helper function to print temperature in X.XXX°C format.
 * Expects a value in milli-degrees Celsius, e.g. 23567 means 23.567°C.
 */
void print_temperature_int_to_float(uint16_t temp)
{
  printf("%u.%03uC\n", temp / 1000, temp % 1000);
}

PROCESS_THREAD(button_press_process, ev, data)
{
  /* Declare an event timer object */
  static struct etimer et;

  PROCESS_BEGIN();
  
  SENSORS_ACTIVATE(button_sensor);
  SENSORS_ACTIVATE(sht11_sensor); // Activate the SHT11 sensor

  while(1) {
    /* Wait until a button press event occurs */
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    // Turn on LED to indicate a reading
    leds_on(LEDS_YELLOW);

    // Read raw temperature from sht11
    int raw_temp = sht11_sensor.value(SHT11_SENSOR_TEMP);

    // Convert raw temperature to Celsius
    float temp_c = -39.60 + 0.01 * raw_temp;
    // Convert to milli-degrees (e.g., 23.567°C -> 23567)
    uint16_t milli_temp = (uint16_t)(temp_c * 1000);

    printf("Temperature: ");
    print_temperature_int_to_float(milli_temp);

    /* Set the etimer for a 2-second delay before turning off the LED.
       CLOCK_SECOND is the number of clock ticks in one second.
       Adjust 2*CLOCK_SECOND to the desired delay duration.
    */
    etimer_set(&et, 2 * CLOCK_SECOND);

    /* Wait for the timer event */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    // Turn off LED after delay
    leds_off(LEDS_YELLOW);
  }

  PROCESS_END();
}

/*
 * Answer/Explanation:
 *
 * Exercise 2a:
 * After compiling and running the program on the sky platform (e.g., using `make TARGET=sky`),
 * press the button on the device. The program will respond by turning on the yellow LED
 * and then printing the current temperature reading to the serial interface. By observing
 * the serial output, you can verify that the button press triggers a temperature reading.
 *
 * Exercise 2b:
 * To make the LED blink instead of staying continuously on, you could use a timer or a
 * loop with `PROCESS_PAUSE()` calls to turn the LED on and off a few times before finally
 * turning it off. Additionally, print "button pressed" each time the button event occurs.
 * For example:
 *
 * - On button press:
 *   - Print "button pressed" to the serial output.
 *   - Blink the LED by turning it on and off several times using `etimer_set()` and waiting
 *     for the timer to expire before toggling the LED state.
 *
 * Exercise 2c:
 * To read the temperature sensor when the button is pressed, we already have the code that
 * activates the SHT11 sensor and reads its value using `sht11_sensor.value(SHT11_SENSOR_TEMP)`.
 * After reading the raw temperature, convert it to a human-readable format using the provided
 * `print_temperature_int_to_float()` function. This will print the temperature in a
 * "X.XXX°C" format to the serial interface each time the button is pressed.
 */
