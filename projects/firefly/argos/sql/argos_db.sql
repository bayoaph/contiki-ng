-- phpMyAdmin SQL Dump
-- version 4.6.6deb5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Feb 08, 2020 at 01:12 PM
-- Server version: 10.0.28-MariaDB-2+b1
-- PHP Version: 7.3.11-1~deb10u1

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `eeb_db`
--
CREATE DATABASE IF NOT EXISTS `argos_db` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `argos_db`;

-- --------------------------------------------------------

--
-- Table structure for table `sensor_data`
--

CREATE TABLE `sensor_data` (
  `logId` int(255) UNSIGNED NOT NULL,
  `date` date NOT NULL,
  `time` time NOT NULL,
  `moteId` int(255) UNSIGNED NOT NULL,
  `temp` double DEFAULT NULL,
  `humd` double DEFAULT NULL,
  `irrd` double DEFAULT NULL,
  `batt` double DEFAULT NULL,
  `ecpu` double DEFAULT NULL,
  `elpm` double DEFAULT NULL,
  `etx` double DEFAULT NULL,
  `erx` double DEFAULT NULL,
  `econ` double DEFAULT NULL,
  `rssi` int(11) DEFAULT NULL,
  `lqi` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Truncate table before insert `sensor_data`
--

TRUNCATE TABLE `sensor_data`;
--
-- Indexes for dumped tables
--

--
-- Indexes for table `sensor_data`
--
ALTER TABLE `sensor_data`
  ADD PRIMARY KEY (`logId`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `sensor_data`
--
ALTER TABLE `sensor_data`
  MODIFY `logId` int(255) UNSIGNED NOT NULL AUTO_INCREMENT;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
