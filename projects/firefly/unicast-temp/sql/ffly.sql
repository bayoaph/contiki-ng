-- MySQL dump 10.16  Distrib 10.1.38-MariaDB, for debian-linux-gnueabihf (armv7l)
--
-- Host: localhost    Database: ffly_db
-- ------------------------------------------------------
-- Server version	10.1.38-MariaDB-0+deb9u1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `sensor_data`
--

DROP TABLE IF EXISTS `sensor_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sensor_data` (
  `logId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `moteId` int(11) NOT NULL,
  `date` date NOT NULL,
  `time` time NOT NULL,
  `temp` float NOT NULL,
  `batt` float NOT NULL,
  `rssi` int(11) NOT NULL,
  PRIMARY KEY (`logId`)
) ENGINE=InnoDB AUTO_INCREMENT=42 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sensor_data`
--

LOCK TABLES `sensor_data` WRITE;
/*!40000 ALTER TABLE `sensor_data` DISABLE KEYS */;
INSERT INTO `sensor_data` VALUES (1,5,'2019-08-02','21:09:23',20.477,3.018,-91),(2,1,'2019-08-02','21:11:28',19.762,2.847,-93),(3,2,'2019-08-02','21:11:41',17.858,2.833,-72),(4,4,'2019-08-02','21:11:58',22.381,2.96,-77),(5,3,'2019-08-02','21:12:17',23.096,2.97,-78),(6,5,'2019-08-02','21:12:39',21.191,3.017,-94),(7,5,'2019-08-02','21:14:39',21.905,3.02,-96),(8,1,'2019-08-02','21:19:29',19.048,2.845,-83),(9,2,'2019-08-02','21:32:41',17.143,2.828,-78),(10,5,'2019-08-02','22:03:40',22.62,3.009,-93),(11,4,'2019-08-02','22:03:59',21.667,2.945,-87),(12,3,'2019-08-02','22:13:17',23.096,2.955,-76),(13,2,'2019-08-02','22:13:41',17.381,2.827,-80),(14,4,'2019-08-02','22:13:59',22.143,2.945,-87),(15,5,'2019-08-02','22:15:40',22.62,3.007,-93),(16,1,'2019-08-02','22:19:29',18.334,2.837,-81),(17,5,'2019-08-02','22:48:39',21.905,2.998,-94),(18,3,'2019-08-02','23:14:17',23.096,2.944,-78),(19,2,'2019-08-02','23:14:41',16.905,2.82,-81),(20,4,'2019-08-02','23:14:58',21.905,2.933,-89),(21,1,'2019-08-02','23:15:29',18.572,2.834,-87),(22,5,'2019-08-03','00:03:39',19.286,2.977,-86),(23,5,'2019-08-03','00:04:39',18.334,2.977,-93),(24,5,'2019-08-03','00:06:39',17.143,2.975,-87),(25,5,'2019-08-03','00:09:39',16.429,2.974,-87),(26,3,'2019-08-03','00:15:17',23.096,2.936,-76),(27,2,'2019-08-03','00:15:41',16.905,2.813,-82),(28,4,'2019-08-03','00:15:58',21.429,2.922,-87),(29,1,'2019-08-03','00:16:29',18.334,2.826,-82),(30,5,'2019-08-03','00:24:39',15,2.972,-85),(31,2,'2019-08-03','00:24:41',16.429,2.813,-81),(32,5,'2019-08-03','00:25:39',15,2.972,-83),(33,2,'2019-08-03','00:32:41',17.143,2.812,-82),(34,5,'2019-08-03','00:39:40',14.286,2.967,-85),(35,2,'2019-08-03','00:57:41',16.429,2.811,-78),(36,3,'2019-08-03','00:58:17',22.381,2.926,-78),(37,4,'2019-08-03','01:16:59',21.667,2.914,-87),(38,3,'2019-08-03','01:17:17',22.858,2.925,-78),(39,1,'2019-08-03','01:17:29',18.096,2.82,-81),(40,2,'2019-08-03','01:19:41',16.667,2.81,-83),(41,5,'2019-08-03','01:27:39',14.048,2.962,-78);
/*!40000 ALTER TABLE `sensor_data` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-08-04 11:04:19
