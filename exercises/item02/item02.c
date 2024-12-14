Answered Exercise Items 1 - 5
a95251e
exercises\item02\item02.c

@@ -5,8 +5,9 @@
#include <stdio.h> /* For printf() */
#include <string.h>
#include "dev/button-sensor.h" /* for the button sensor */
#include "dev/zoul-sensors.h"  /* for the temperature sensor */
#include "node-id.h"           /* get the variable node_id that holds the own node id */
#include "arch/dev/sensor/sht11/sht11-sensor.h"
//#include "dev/zoul-sensors.h"  /* for the temperature sensor */
#include "sys/node-id.h"           /* get the variable node_id that holds the own node id */
#include "dev/leds.h"
/*---------------------------------------------------------------------------*/
PROCESS(button_press_process, "Button Press Process");

@@ -22,21 +23,25 @@ PROCESS_THREAD(button_press_process, ev, data)
  PROCESS_BEGIN();

  SENSORS_ACTIVATE(button_sensor);
  SENSORS_ACTIVATE(sht11_sensor);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

    leds_on(LEDS_BLUE);

    leds_on(LEDS_RED);
    printf("button pressed");
    printf("Temperature: ");
    SENSORS_ACTIVATE(cc2538_temp_sensor);
    print_temperature_int_to_float(cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED));
    SENSORS_DEACTIVATE(cc2538_temp_sensor); 
    print_temperature_int_to_float(sht11_sensor.value(SHT11_SENSOR_TEMP));

    leds_off(LEDS_BLUE);
    leds_off(LEDS_RED);
  }
