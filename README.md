# Contiki Projects
---
Principal author: Mark Anthony Cabilo
Contributors: ...
Created: April 2019
Restored: October 2024
---
Brief:<br>
Projects based on Contiki-NG. 

---
**Installation**
1. Run `install_dependencies.sh` with ```./install_dependencies.sh```
2. Reboot system.
3. Run `install_contiki_ng.sh` with ```./install_contiki_ng.sh```
4. Initiate `main_contiki` submodule.

---
**If running WSL**

---
**Sample Projects**

---
**Usage**
1. Defining `target`, `board`, and `port`. You can create a file named `Makefile.target` to automatically feed target to your build commands. Put this file on the project folder root. The content of the file may look like as follows:
```
TARGET = zoul
BOARD = firefly
PORT=/dev/ttyUSB0
```
2. If `Makefile` is defined, use the following command to compile source files in the project folder:
```
make
```
3. If `Makefile` is defined, use the following command to compile and upload code into your target device:
```
make <nameOfFile>.upload
```
4. To access the device's serial after upload, use the following command:
```
make login
```