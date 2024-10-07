# Contiki Projects
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
**Usage**
For most projects with configured `Makefile` and `Makefile.target` files, the following commands apply:
1. Compiling: 
```
cd <path_to_your_project> 
$ make 
```
2. Uploading (same directory):
```
$ make <src_name>.upload
```
3. Viewing logs on serial (same directory):
```
$ make login
```
---
**Setting up Makefile.target**
`Makefile.target` is usefull in directing the compiler the intended device for your programs. 
1. In the root folder of your project, create `Makefile.target`.
```
$ cd <path_to_your_project>
$ touch Makefile.target
```
2. Inside the file, define the target IoT device, specific board, and port where it is plugged in. The example below is for Zolertia Firefly motes.
```
$ nano Makefile.target
{Inside the file}
TARGET=zoul
BOARD=firefly
PORT=/dev/ttyUSB0
```
From here, you can compile or upload codes as mentioned above.

---
Principal author: Mark Anthony Cabilo
Contributors: ...
Created: April 2019
Restored: October 2024