CONTIKI_PROJECT = finalExam
TARGET=sky
all: $(CONTIKI_PROJECT)

# DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_MAC=csma_driver
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

CONTIKI = /home/carl/wyndel/contiki-ng/main_contiki
include $(CONTIKI)/Makefile.include
CFLAGS += -I$(CONTIKI)/os