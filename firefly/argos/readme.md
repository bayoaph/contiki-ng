**Argos - Building Ambience Monitoring Application**

Instructions:
1. Upload the two codes to corresponding motes. *sensor.c* is the code for sensors collecting data.
*sink.c* is the code for the base station mote.
2. Node IDs / Mote Addresses should be defined when uploading the code: 
ex: make sensor.upload NODEID=0x0023 PORT=/dev/ttyUSB0

***************************************************************************************************************************
To run database storage python script automatically and in background, refer to the following steps:
1. Copy eeb-mon.service to /etc/systemd/system
2. Create the following folder /home/pi/argos_data/ and set its permission to 777
3. Edit /etc/mysql/my.cnf and add a line "tmp =/home/pi/argos_data" at the end (only if the line does not exist)
4. Restart mysql (sudo service mysql restart)
5. Enable service by "sudo systemctl enable eeb-mon" (service starts automatically at next reboot)
6. Check service status by "sudo systemd status eeb-mon"
7. Stop service by "sudo systemctl stop eeb-mon" or disable it by "sudo systemctl disable eeb-mon"

To start service immediately, "sudo systemd start eeb-mon"

It is also possible to perform the start, stop, and status function using "sudo service eeb-mon [start/stop/status]"

****************************************************************************************************************************
We use the dbstore2.py file to incorporate new functions. REMEMBER: Only save dbstore2.py as dbstore.py when the former is
running without bugs to prevent unnecessary errors during service runtime!

***************************************************************************************************************************
