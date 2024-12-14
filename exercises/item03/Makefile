CONTIKI_PROJECT = item03_3a item03_3b
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1

# Add necessary source files for Z1 platform
PROJECT_SOURCEFILES += adxl345.c battery-sensor.c button-sensor.c \
                       cc2420-arch.c i2cmaster.c light-sensor.c \
                       light-ziglet.c potentiometer-sensor.c \
                       reed-sensor.c relay-phidget.c sht25.c \
                       sky-sensors.c temperature-sensor.c \
                       tlc59116.c tmp102.c xmem.c z1-phidgets.c z1-sensors.c

# Configure the network stack
DEFINES = NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

CONTIKI = ../../main_contiki
include $(CONTIKI)/Makefile.include
