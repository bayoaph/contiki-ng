import threading
import time
import serial
from datetime import datetime as realTime
import MySQLdb as databaseDriver
#import thingspeak
import logging

logging.basicConfig(filename='/home/pi/eeb.log',level=logging.INFO)

channelID=[0,0,0,0,0,985194,0,984219]
api_w=[" "," "," "," "," ","5SBH0O6OFDJMGNPU"," ","RRSNFXD754I2O0L4"]

PORT='/dev/ttyUSB0'
#PORT='/dev/ttyACM0'

sinkConnection = serial.Serial(port=PORT, baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS,timeout=1)

shutdownEvent = threading.Event()

def extract(line):
	try:
		item=line.split(",")
		mote=int(item[0])
		batt=float(item[1])/1000.0
		temp=float(item[2])/1000.0
		ecpu=float(item[3])
		elpm=float(item[4])
		etx=float(item[5])
		erx=float(item[6])
		rssi=int(item[7])
		lqi=int(item[8])
	except Exception as error:
		currentTime=realTime.now()
		logging.info("["+str(currentTime.time())+" "+str(currentTime.date())+"] Error:"+error)
		mote=0
		batt=0
		temp=0
		ecpu=0
		elpm=0
		etx=0
		erx=0
		rssi=0
		lqi=0
	econ = ecpu+elpm+etx+erx
	return (mote,temp,batt,ecpu,elpm,etx,erx,econ,rssi,lqi)

def saveData(name, chList):
	logging.info("["+name+"] Starting thread")
	global lastUpdateTime

	while not shutdownEvent.is_set():
		#check serial for new data
		line = sinkConnection.readline()
		#if new serial data is available
		if(line):
			logging.info("[Firefly]: new data available in serial")
			#extract individual data values from serial stream
			(mote,temp,batt,ecpu,elpm,etx,erx,econ,rssi,lqi)=extract(line)
			#convert individual data values into a string for database storage
			dataStr=(mote,temp,batt,ecpu,elpm,etx,erx,econ,rssi,lqi)
			#if data came from a valid mote (nonzero mote ID)
			if mote > 0:
				#open database and submit data insert query
				try:
					databaseLink = databaseDriver.connect("localhost","admin","apollo","eeb_db")
					databaseCursor = databaseLink.cursor()
					databaseCursor.execute("INSERT INTO sensor_data(logId,moteId,date,time,temp,humd,batt,ecpu,elpm,etx,erx,econ,rssi,lqi) VALUES (NULL,%s,CURDATE(),CURTIME(),%s,NULL,%s,%s,%s,%s,%s,%s,%s,%s)" ,dataStr)
				except Exception as error:
					#if an error happens during connection or query, rollback and display error!
					databaseLink.rollback()
					currentTime = realTime.now()
					logging.info("["+str(currentTime.time())+" "+str(currentTime.date())+"] Error: "+str(error))
				else:
					databaseLink.commit()
					currentTime=realTime.now()
					logging.info("["+str(currentTime.time())+" "+str(currentTime.date())+"] Data from mote "+str(mote)+" saved")
				finally:
					databaseCursor.close()
					databaseLink.close()

#				updateChannel(chList[mote], temp, batt, ecpu, elpm, etx, erx, rssi, lqi)
		else:
#			print "[Python]: sleeping"
			time.sleep(50/1000)

	logging.info("["+name+"] Exiting thread")

def updateChannel(device, temp, batt, ecpu, elpm, etx, erx, rssi, lqi):
	try:
		response = device.update({'field1':temp,'field2':batt,'field3':ecpu,'field4':elpm,'field5':etx,'field6':erx,'field7':rssi,'field8':lqi})
	except Exception as error:
		logging.info("[Python] Error saving to thingspeak: "+str(error))
	else:
		logging.info("[Python]: export to Thingspeak (HTTP response: "+str(response)+")")

def exportData(name):
	logging.info("["+name+"] Starting thread")
	reset = 0
	skipTruncate = 0

	# while the program was not halted, perform function
	while not shutdownEvent.is_set():
		#get current time to check if midnight
		currentTime = realTime.now()

		#export csv within RPi
		if str(currentTime.hour)== '12' and reset == 0:
			try:
				databaseLink = databaseDriver.connect("localhost","admin","apollo","eeb_db")
				databaseCursor = databaseLink.cursor()
				docStr = ("tmp/sdata-{}-{}".format(currentTime.month,currentTime.day),count)
				databaseCursor.execute("SELECT * INTO OUTFILE '%s.csv' FIELDS TERMINATED BY ',' FROM sensor_data WHERE mote=",docStr)
			except Exception as error:
				databaseLink.rollback()
				newTime = realTime.now()
				logging.info("["+str(newTime.time())+" "+str(newTime.date())+"] Error: "+str(error))
				skipTruncate = 1
			else:
				databaseLink.commit()
				newTime = realTime.now()
				logging.info("["+str(newTime.time())+" "+str(newTime.date())+"] CSV file generated")
				reset = 1
				skipTruncate = 0

			if not skipTruncate:
				try:
					databaseCursor.execute("TRUNCATE sensor_data")
				except Exception as error:
					databaseLink.rollback()
					newTime = realTime.now()
					logging.info("["+str(newTime.time())+" "+str(newTime.date())+"] Error: "+str(error))
				else:
					databaseLink.commit()
					newTime = realTime.now()
					logging.info("["+str(newTime.time())+" "+str(newTime.date())+"] Database emptied")

			databaseCursor.close()
			databaseLink.close()
		elif str(currentTime.hour != '12') and reset == 1:
			reset = 0

	logging.info("["+name+"] Exiting thread")

if __name__=="__main__":
#	ch5 = thingspeak.Channel(id=channelID[5],write_key=api_w[5])
#	ch7 = thingspeak.Channel(id=channelID[7],write_key=api_w[7])
#	chList=[None,None,None,None,None,ch5,None,ch7,None]

	t1 = threading.Thread(target=saveData, args=("StoreThread", chList))
	t2 = threading.Thread(target=exportData, args=("ExportThread",))

	t1.start()
	t2.start()

	try:
		while t1.isAlive():
			t1.join(timeout=1.0)
	except(KeyboardInterrupt, SystemExit):
		shutdownEvent.set()
	pass

