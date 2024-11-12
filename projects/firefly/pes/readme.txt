Prepaid Electricity Application

/* Development Files */
The 'hw_datasheet' folder contains the datasheets of involved hardware components such as:
1. LCD2004 LCD-I2C Interface 
2. PCF8574 I/O I2C Expander used by LCD2004
2. HD44780 Hitachi LCD Module

//To be updated (last edit March 2021)
1. Upload the two codes to corresponding motes. *sensor.c* is the code for sensors collecting data.
*sink.c* is the code for the base station mote.
2. Hardcoded for transmission, base station has an address of 0.255 so you need to assign this address to the sink mote.

***************************************************************************************************************************
To run database storage python script automatically and in background, refer to the following steps:
1. Copy eeb-temp.service to /etc/systemd/system
2. Enable service by "sudo systemctl enable eeb-temp" (service starts at next reboot)
3. Check service status by "sudo systemd status eeb-temp"
4. Stop service by "sudo systemctl stop eeb-temp" or disable it by "sudo systemctl disable eeb-temp"

To start service immediately, "sudo systemd start eeb-temp"
