import sys
import threading
import time
import serial
from datetime import datetime as realTime
import MySQLdb as databaseDriver
import thingspeak
import logging
from logging.handlers import RotatingFileHandler #for log files management
import inspect #for error detection
import pydrive.auth
from pydrive.auth import GoogleAuth
from pydrive.drive import GoogleDrive
import os
import fnmatch
import json

#prevent detection failure of client_secrets.json
os.chdir("/home/pi/contiki-3.0/contiki-projects/firefly/argos")

#remove unnecessary import error prompt from discovery_cache
logging.getLogger('googleapiclient.discovery_cache').setLevel(logging.ERROR)

############################################################# EDITABLE LINES #######################################################################
#!! BE CAREFUL WHEN RENAMING THESE STRINGS AS WRONG INPUTS CAN EITHER SAVE FILES IN WRONG FOLDERS OR FAILURE TO SAVE DATA AT ALL!!##################
bldg_file = "C001_spec.json" # CTU-M Graduate School Building
set_hour_exp = '0'
set_minute_exp = '0'
set_hour_upld = '9'
set_minute_upld = '0'
RPI_UNIT="eeb01"
####################################################################################################################################################

channelID=[0,0,0,0,0,1338776,0,1338881]
api_w=[" "," "," "," "," ","99VRLHZBEBJ77H2B"," ","KWCLALZ56O58CSLF"]

PORT=sys.stdin.read().replace('\n','') # read name of port where sink node is connected
sinkConnection = serial.Serial(port=PORT, baudrate=115200, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS,timeout=1)
shutdownEvent = threading.Event()

############################################################ Debugging #############################################################################
def setupLogging(log_dir):
	log_file_format = "<<%(levelname)s>>[%(asctime)s][%(name)s.%(funcName)s:%(lineno)d] %(message)s"
	log_console_format = "[%(levelname)s]: %(message)s"
	date_format = "%Y-%m-%dT%H:%M:%S"

	global main_logger
	main_logger = logging.getLogger()
	main_logger.setLevel(logging.DEBUG)

#	Avoid conflict with serial data from Firefly
	console_handler = logging.StreamHandler()
	console_handler.setLevel(logging.DEBUG)
	console_handler.setFormatter(logging.Formatter(log_console_format))

	exp_file_handler = RotatingFileHandler('{}argos_debug.log'.format(log_dir), maxBytes=50*1024, backupCount=10)
	exp_file_handler.setLevel(logging.DEBUG)
	exp_file_handler.setFormatter(logging.Formatter(fmt=log_file_format, datefmt=date_format))

	exp_errors_file_handler = RotatingFileHandler('{}argos_error.log'.format(log_dir), maxBytes=50*2014, backupCount=10)
	exp_errors_file_handler.setLevel(logging.WARNING)
	exp_errors_file_handler.setFormatter(logging.Formatter(fmt=log_file_format, datefmt=date_format))

	main_logger.addHandler(console_handler)
	main_logger.addHandler(exp_file_handler)
	main_logger.addHandler(exp_errors_file_handler)

############################################################## Google Drive Access ##################################################################
def getDrive(drive=None, gauth=None):
	if not drive:
		if not gauth:
			gauth=GoogleAuth()

		gauth.LoadCredentialsFile("credentials.txt")

		if gauth.credentials is None:
			try:
				gauth.LocalWebserverAuth()
			except pydrive.auth.AuthError as e:
				main_logger.error("Credential creation error: %s", e)
			except Exception as e:
				main_logger.exception(e)
			else:
				main_logger.debug("New credentials created.")
		elif gauth.access_token_expired:
			try:
				gauth.Refresh()
			except pydrive.auth.RefreshError as e:
				main_logger.error("Google Drive error: %s", e)
			except Exception as e:
				main_logger.exception(e)
			else:
				main_logger.debug("Auth token refreshed.")
		else:
			try:
				gauth.Authorize()
			except pydrive.auth.AuthError as e:
				main_logger.error("Authorization error: %s", e)
			else:
				main_logger.debug("Authentication successful!")

		gauth.SaveCredentialsFile("credentials.txt")
		return GoogleDrive(gauth)

	if drive.auth.access_token_expired:
		try:
			drive.auth.Refresh()
		except pydrive.auth.RefreshError as e:
			main_logger.error("Google Drive error: %s", e)
		else:
			main_logger.info("Refreshed drive auth")
	return drive

########################################################### Building-Specific Parameters ############################################################
def loadSettings():
	try:
		file = open(bldg_file,)
		bldg_data = json.load(file)

		global folder_id
		global building_name
		folder_id=bldg_data["folder_id"]
		building_name=bldg_data["name"]
	except OSError:
		main_logger.error("Cannot open "+str(bldg_file))
	else:
		file.close()

########################################################### WSN data processing #####################################################################
# extract and convert individual data from string packet into float or int data type
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
		hops=int(item[9])
	except Exception as error:
		currentTime=realTime.now()
		main_logger.error("Extraction error: %s", error)
		mote=0
		batt=0
		temp=0
		ecpu=0
		elpm=0
		etx=0
		erx=0
		rssi=0
		lqi=0
		hops=0
	econ = ecpu+elpm+etx+erx
	return (mote,temp,batt,ecpu,elpm,etx,erx,econ,rssi,lqi,hops)

# checks incoming transmissions and saves data into the local SQL database
def saveData(name, chList):
	global lastUpdateTime

	while not shutdownEvent.is_set():
		#check serial for new data
		line = sinkConnection.readline()
		#if new serial data is available
		if(line):
			#extract individual data values from serial stream
			(mote,temp,batt,ecpu,elpm,etx,erx,econ,rssi,lqi,hops)=extract(line)
			#convert individual data values into a string for database storage
			dataStr=(mote,temp,batt,ecpu,elpm,etx,erx,econ,rssi,lqi,hops)
			main_logger.debug("New data received from mote %d", mote)
			#if data came from a valid mote (nonzero mote ID)
			if mote > 0:
				#open database and submit data insert query
				try:
					databaseLink = databaseDriver.connect("localhost","admin","2f24FF!mdb93","argos_db")
					databaseCursor = databaseLink.cursor()
					databaseCursor.execute("INSERT INTO sensor_data(logId,moteId,date,time,temp,humd,batt,ecpu,elpm,etx,erx,econ,rssi,lqi,hops) VALUES (NULL,%s,CURDATE(),CURTIME(),%s,NULL,%s,%s,%s,%s,%s,%s,%s,%s,%s)" ,dataStr)
				except databaseDriver.Error as e:
					main_logger.error("During writing data to db: %s", e)
					databaseLink.rollback()
				except Exception as e:
					main_logger.exception(e)
				else:
					databaseLink.commit()
					main_logger.debug("Data from mote %d saved", mote)
				finally:
					databaseCursor.close()
					databaseLink.close()
				if mote == 129:
					updateChannel(chList[5], temp, batt, ecpu, elpm, etx, erx, rssi, lqi)
				if mote == 84:
					updateChannel(chList[7], temp, batt, ecpu, elpm, etx, erx, rssi, lqi)
		else:
			time.sleep(50/1000)

# sends data to ThingSpeak with corresponding WSN node ID to distinguish data from each other
def updateChannel(device, temp, batt, ecpu, elpm, etx, erx, rssi, lqi):
	try:
		response = device.update({'field1':temp,'field2':batt,'field3':ecpu,'field4':elpm,'field5':etx,'field6':erx,'field7':rssi,'field8':lqi})
	except Exception as e:
		main_logger.exception(e)
	else:
		main_logger.debug("Export to Thingspeak (HTTP response: "+str(response)+")")

# once daily, ask local database to export data to a CSV file and store it to /home/pi/data directory
def exportData(name):
	recorded = False
	skipTruncate = 0
	count = 0

	# while the program was not halted, perform function
	while not shutdownEvent.is_set():
		#get current time to check if midnight
		currentTime = realTime.now()

		#export csv within RPi (every 12 midnight and once exported successfully, stop trying to export during the entire minute)
		# if exporting fails within the entire minute, there will be no exported data file for the previous day
		if str(currentTime.hour) == set_hour_exp and str(currentTime.minute) == set_minute_exp:
			if recorded == False:
				try:
					databaseLink = databaseDriver.connect("localhost","admin","2f24FF!mdb93","argos_db")
					databaseCursor = databaseLink.cursor()
					count = count + 1
					docStr = "argos-{}-rawdata-{:0>2}{:0>2}{:0>2}-{:0>2}{:0>2}-{:0>4}.csv".format(RPI_UNIT,str(currentTime.year),str(currentTime.month),str(currentTime.day),str(currentTime.hour),str(currentTime.minute),str(count))
					databaseCursor.execute("SELECT * INTO OUTFILE '/tmp/%s' FIELDS TERMINATED BY ',' FROM sensor_data" % (docStr))
				except databaseDriver.Error as e:
					databaseLink.rollback()
					main_logger.error("Error exporting data: %s ", e)
					skipTruncate = 1
				except Exception as e:
					main_logger.exception(e)
					skipTruncate = 1
				else:
					databaseLink.commit()
					main_logger.debug("CSV file generated")
					skipTruncate = 0

				if not skipTruncate:
					try:
						databaseCursor.execute("TRUNCATE sensor_data")
					except databaseDriver.Error as e:
						databaseLink.rollback()
						main_logger.error("Error during truncating db: %s ", e)
					except Exception as e:
						main_logger.exception(e)
					else:
						databaseLink.commit()
						main_logger.debug("Database emptied")

				databaseCursor.close()
				databaseLink.close()
				recorded = True
		elif str(currentTime.hour) != set_hour_exp and recorded == True:
			recorded = False
		if str(currentTime.hour) == set_hour_upld and str(currentTime.minute) == set_minute_upld:
			if recorded == False:
				updateFileUpload(docStr)
				recorded = True


####################################################################### GDRIVE PROCESSING ###############################################################
def updateFileUpload(doc=None):
	newTime = realTime.now()

	file = "/tmp/"+doc
	gfile = drive.CreateFile({'title': doc, 'parents': [{'id':folder_id}]})
	gfile.SetContentFile(file)
	gfile.Upload()
	main_logger.info("Pushed %s to %s GDrive folder!", doc, building_name)

	logs = findLog(".log.*", "./logs")
	for log in logs:
		gfile = drive.CreateFile({'title':log, 'parents':[{'id':folder_id}]})
		gfile.SetContentFile(file)
		gfile.Upload()
		main_logger.info("Saved log file %s to %s GDrive folder!", log, building_name)


def findLog(exp, path):
	result = []
	for root, dirs, files in os.walk(path):
		for name in files:
			if fnmatch.fnmatch(name, exp):
				result.append(os.path.join(root, name))
	return result

######################################################################### MAIN FUNCTION #################################################################

# main function! we run two threads, 1 for intercepting incoming transmissions and the other to export data from database;
# creating two threads makes sure that no incoming transmission is missed out during the database export process
if __name__=="__main__":
	#setup logging settings
	setupLogging("./logs/")

	#load building-specific configurations
	loadSettings()

	drive = getDrive()

	ch5 = thingspeak.Channel(id=channelID[5],write_key=api_w[5])
	ch7 = thingspeak.Channel(id=channelID[7],write_key=api_w[7])
	chList=[None,None,None,None,None,ch5,None,ch7,None]

	t1 = threading.Thread(target=saveData, args=("StoreThread", chList))
	t2 = threading.Thread(target=exportData, args=("ExportThread",))

	t1.start()
	t2.start()

	try:
		while t1.isAlive():
			t1.join(timeout=1.0)
	except(KeyboardInterrupt, SystemExit):
		shutdownEvent.set()
	except Exception as e:
		main_logger.exception(e)
	pass

