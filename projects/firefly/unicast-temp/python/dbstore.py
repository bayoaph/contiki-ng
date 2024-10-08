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
		rssi=int(a[3])
	except Exception as error:
		now = datetime.datetime.now()
		print ("["+str(now.time())+" "+ str(now.date())+"] Error: {}".format(error))
		mote = 0;
		batt = 0;
		temp = 0;
		rssi = 143;
	return (mote, temp, batt, rssi)

def main():
	reset = 0
	while True:
		line=ser.readline()
		if(line):
			(mote,temp,batt,rssi) = extract(line)
			data_str = (mote, temp, batt, rssi)
			now = datetime.datetime.now()
		if rssi < 143:
			try:
				db = MySQLdb.connect(host="localhost", user="admin", passwd="apollo", db="ffly_db")
				cur = db.cursor()
				cur.execute("""INSERT INTO sensor_data (logId, moteId, date, time,  temp,  batt, rssi) VALUES (NULL, %s, CURDATE(), CURTIME(), %s, %s, %s)""", data_str)
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
		if (str(now.hour) == '0') and (str(now.minute) == '0') and (reset == 0):
			try:
				db = MySQLdb.connect(host="localhost", user="admin", passwd="apollo", db="ffly_db")
				cur = db.cursor()
				cur.execute("SELECT * INTO OUTFILE '/tmp/" + str(now.year) + "-" + str(now.month) +"-"+ str(now.day) +".csv' FIELDS TERMINATED BY ',' FROM sensor_data")
				db.commit()
				cur.execute("TRUNCATE sensor_data")
			except(KeyboardInterrupt, SystemExit):
				db.rollback()
			except MySQLdb.Error as error:
				print("["+str(now.time())+" "+str(now.date())+"] MySQL error: {}".format(error))
			else:
				db.commit()
				print("["+str(now.time())+" "+str(now.date())+"] CSV file exported to /tmp folder")
			finally:
				cur.close()
				db.close()
			reset = 1
		elif (reset == 1 and str(now.minute) != '0'):
			reset = 0

if __name__ == "__main__":
	main()
