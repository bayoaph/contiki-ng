Sample Powertrace Application

1. Upload the two codes to corresponding motes. *sensor.c* is the code for sensors collecting data.
*sink.c* is the code for the base station mote.

2. Hardcoded for transmission, base station has an address of 0.255 so you need to assign this address to the sink mote.

3. Don't forget to make sure that ENERGEST_CONF_ON is defined as 1 in contiki-conf.h

