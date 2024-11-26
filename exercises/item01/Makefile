CONTIKI_PROJECT = item01
all: $(CONTIKI_PROJECT)

CFLAGS += -DWITH_UIP=1

# DEFINES for network stack
DEFINES += NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

# Path to Contiki-NG (adjust this if necessary)
CONTIKI = /home/kcerio/IoT/contiki-ng/_setup/main_contiki

# Include the Makefile from Contiki-NG
include $(CONTIKI)/Makefile.include
