# -*- coding: utf-8 -*-
# -------------------------------------------------------------------------- #
# Simple application to relay data to Ubidots from a Contiki serial-based conn
# -------------------------------------------------------------------------- #

import serial
from time import sleep
from ubidots import ApiClient

#Use as default
PORT = "/dev/ttyUSB0"

def connectMote(port):
	try:
		ser = serial.Serial(port, 115200, timeout=0, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
	except:
		sys.exit("Error in connecting to the USB device, aborting")
	return ser

def process_data(raw):
	if "***" in raw:
		raw = raw[(raw.index("***")+3):]
		if "***" in raw:
			raw = raw[:raw.index("***")]
			ubidots_raw = raw.split("\t")
			client = ApiClient(ubidots_raw[0])
			try:
				my_variable = client.get_variable(ubidots_raw[1])
			except Exception, e:
				print "Ubidots error: %s" % e
				return
			my_variable.save_value({'value':int(ubidots_raw[2])})

if __name__=='__main__':
	s = connectMote(PORT)
	while True:
		queue = s.inWaiting()
		if queue > 0:
			data = s.read(1000)
			process_data(data)
		sleep(0.2)
