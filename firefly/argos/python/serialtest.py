import serial
import MySQLdb
import datetime

PORT='/dev/ttyUSB0'
ser = serial.Serial(port=PORT, baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=None)

def extract(line):
	try:
		a = line.split(",")
		mote = int(a[0])
		batt= float(a[1])/1000.0
		temp=float(a[2])/1000.0
		low=int(a[3])
		rssi=int(a[4])
	except Exception as error:
		now = datetime.datetime.now()
		print ("["+str(now.time())+" "+ str(now.date())+"] Error: {}".format(error))
		mote = 0
		batt = 0
		temp = 0
		low = -1
		rssi = 99
	return (mote, temp, batt, low, rssi)

def main():
	reset = 0
	print ("Init...")
	while True:
		line=ser.readline()
		if(line):
			#(mote,temp,batt,low,rssi) = extract(line)
			#data_str = (mote, temp, batt, low, rssi)
			print (line)
		else:
			print("no data")


if __name__ == "__main__":
	main()
