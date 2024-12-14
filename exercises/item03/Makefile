CONTIKI_PROJECT = item03
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1

# DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_MAC=csma_driver
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

CONTIKI = /home/karen/contiki-ng/_setup/main_contiki
include $(CONTIKI)/Makefile.include

CFLAGS += -I$(CONTIKI)/os
APPS += simple-udp
