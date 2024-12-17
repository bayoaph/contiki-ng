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
  /* Print in a human-readable format: 
     If temp = 6000, this prints "6.000C" */
  printf("%u.%03uC\n", temp / 1000, temp % 1000);
}

PROCESS_THREAD(button_press_process, ev, data)
{
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_GREEN);
    printf("button pressed\n");
    printf("Temperature: ");

    SENSORS_ACTIVATE(sht11_sensor);
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));
    SENSORS_DEACTIVATE(sht11_sensor);

    leds_off(LEDS_GREEN);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&button_press_process);
/*---------------------------------------------------------------------------*/
