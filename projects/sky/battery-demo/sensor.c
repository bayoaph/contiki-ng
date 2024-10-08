#include "contiki.h"
#include "dev/battery-sensor.h"
#include <stdio.h>

PROCESS(batt, "Battery sensing for sky");
AUTOSTART_PROCESSES(&batt);

PROCESS_THREAD(batt, ev, data){
	static struct etimer et;

	PROCESS_BEGIN();
	SENSORS_ACTIVATE(battery_sensor);

	etimer_set(&et, CLOCK_SECOND);
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		printf("Batt level: %umV\n", battery_sensor.value(1));
		etimer_reset(&et);
	}
	PROCESS_END();
}
