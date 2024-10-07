#!/bin/bash

ls /dev/ttyUSB* | python ~/contiki-ng/firefly/argos/argos_main.py


while true
do
	sleep 1
done

