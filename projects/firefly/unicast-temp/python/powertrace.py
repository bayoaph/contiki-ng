#!/bin/python
import serial
import MySQLdb
import datetime

PORT='/dev/ttyUSB0'
ser = serial.Serial(port=PORT, baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS,timeout=None)

def extract(line):
	a=line.split(' ')
#	time0 = int(a[0])
#	addr = str(a[2])+"."+str(a[3])
#	seqno = int(a[4])
	cpu_sum = int(a[5])
	lpm_sum = int(a[6])
	tx_sum = int(a[7])
	rx_sum = int(a[8])
	tx_idle_sum = int(a[9])
	rx_idle_sum = int(a[10])
#	cpu_cur = int(a[11])
#	lpm_cur = int(a[12])
#	tx_cur = int(a[13])
#	rx_cur = int(a[14])
#	tx_idle_cur = int(a[15])
#	rx_idle_cur = int(a[16])
#	tx_dc = 100*(tx_cur + rx_cur)/(cpu_cur + lpm_cur+0.000001)
#	conv = 3.3/327680
#	cpu = cpu_cur*20*conv
#	lpm = lpm_cur*0.0013*conv
#	tx = tx_cur*24*conv
#	rx = rx_cur*20*conv
	return (cpu_sum,lpm_sum,tx_sum,rx_sum,tx_idle_sum,rx_idle_sum)

def connectdb():
	db = MySQLdb.connect(host="localhost", user="admin", passwd="apollo", db="test_db")
	cur = db.cursor()
	return (db, cur)

def main():
	reset = 0
	while True:
		line=ser.readline()
		if(line):
#			(dc, cpu, lpm, tx, rx) = extract(line)
			(cpu, lpm, tx, rx, txi, rxi)=extract(line)
			print ("cpu:{:05.3f} mW, lpm:{:05.3f}mW, tx:{:05.3f}mW, rx:{:05.3f}mW".format(cpu, lpm, tx, rx))
			data_str = (cpu,lpm,tx,rx,txi,rxi)
			try:
				(db, cur)=connectdb()
				cur.execute("""INSERT INTO test_data (logId, date, time, cpu, lpm, tx, rx,txi,rxi) VALUES (NULL, CURDATE(), CURTIME(), %s, %s, %s, %s, %s,%s)""", data_str)
			except(KeyboardInterrupt, SystemExit):
				db.rollback()
			except MySQLdb.Error as error:
				now=datetime.datetime.now()
				print("[{} {}] SQL error: {}".format(now.time(), now.date(), error))
			else:
				db.commit()
				now=datetime.datetime.now()
				print("[{} {}] data logged.".format(now.time(),now.date()))
			finally:
				cur.close()
				db.close()
			now = datetime.datetime.now()
			if(str(now.hour)=='0') and(str(now.minute)=='0') and (reset==0):
				file_str = (now.year(), now.month(), now.day())
				try:
					(db, cur)=connectdb()
					cur.execure("SELECT * INTO OUTFILE '/tmp/test-%s-%s-%s.csv' FIELDS TERMINATED BY ',' FROM test_data", file_str)
					db.commit()
					cur.execute("TRUNCATE test_data")
				except MySQLdb.Error as error:
					print ("[{} {}] SQL Error: {}".format(now.time(), now.date(), error))
				else:
					db.commit()
					print ("[{} {}] Database exported.".format(now.time(), now.time()))
				finally:
					cur.close()
					db.close()
				reset = 1
			elif(reset==1 and str(now.minute) != '0'):
				reset = 0

if __name__=="__main__":
	main()
