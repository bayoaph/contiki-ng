#!/bin/bash

python /home/pi/contiki-3.0/contiki-projects/firefly/eeb/dbstore.py >> /home/pi/contiki-3.0/contiki-projects/firefly/rssi-check/service.log

while true
do
	sleep 1
done

