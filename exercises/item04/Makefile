CONTIKI_PROJECT = item01
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1
CFLAGS += -Wno-error=unused-result

# DEFINES for network stack
DEFINES += NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

# Path to Contiki-NG (adjust this if necessary)
CONTIKI = ../../main_contiki

# Include the Makefile from Contiki-NG
include $(CONTIKI)/Makefile.include

CFLAGS += -I$(CONTIKI)/os

