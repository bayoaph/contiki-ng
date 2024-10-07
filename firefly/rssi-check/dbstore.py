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
#		temp=float(a[2])/1000.0
		ecpu=float(a[2])/1000.0
		elpm=float(a[3])/1000.0
		etx=float(a[4])/1000.0
		erx=float(a[5])/1000.0
		econ=float(a[6])/1000.0
		rssi=int(a[7])
	except Exception as error:
		now = datetime.datetime.now()
		print ("["+str(now.time())+" "+ str(now.date())+"] Error: {}".format(error))
		mote = 0
		batt = 0
		#temp = 0
		ecpu = -1
		elpm = -1
		etx = -1
		erx = -1
		econ = -1
		rssi = 99
	return (mote, batt, ecpu, elpm, etx, erx, econ, rssi)

def main():
	reset = 0
	while True:
		line=ser.readline()
		if(line):
			(mote,batt,ecpu,elpm,etx,erx,econ,rssi) = extract(line)
			data_str = (mote, batt, ecpu, elpm, etx, erx, econ, rssi)
			now = datetime.datetime.now()
		if rssi < 99 :
			try:
				db = MySQLdb.connect(host="localhost", user="admin", passwd="apollo", db="eeb_db")
				cur = db.cursor()
				cur.execute("""INSERT INTO sensor_data (logId, moteId, date, time,  temp,  humd, batt, ecpu, elpm, etx, erx, econ, rssi) VALUES (NULL, %s, CURDATE(), CURTIME(), NULL, NULL, %s, %s, 
%s, %s, %s, %s, %s)""", data_str)
			except(KeyboardInterrupt, SystemExit):
				db.rollback()
			except MySQLdb.Error as error:
				now = datetime.datetime.now()
				print("["+str(now.time())+" "+str(now.date())+"] MySQL error: {}".format(error))
			else:
				db.commit()
				now = datetime.datetime.now()
				print("["+str(now.time())+" "+str(now.date())+"] Data from mote "+str(mote)+" saved.")
			finally:
				cur.close()
				db.close()

		now = datetime.datetime.now()
		if (str(now.hour) == '0')  and (reset == 0):
			try:
				db = MySQLdb.connect(host="localhost", user="admin", passwd="apollo", db="eeb_db")
				cur = db.cursor()
				cur.execute("SELECT * INTO OUTFILE '/tmp/rawdata-" + str(now.year) + "-" + str(now.month) +"-"+ str(now.day) +".csv' FIELDS TERMINATED BY ',' FROM sensor_data")
				db.commit()
				cur.execute("TRUNCATE sensor_data")
			except(KeyboardInterrupt, SystemExit):
				db.rollback()
			except MySQLdb.Error as error:
				print("["+str(now.time())+" "+str(now.date())+"] MySQL error: {}".format(error))
			else:
				db.commit()
				print("["+str(now.time())+" "+str(now.date())+"] CSV file exported to /tmp folder")
				reset = 1
			finally:
				cur.close()
				db.close()
		elif (reset == 1 and str(now.minute) != '0'):
			reset = 0

if __name__ == "__main__":
	main()
