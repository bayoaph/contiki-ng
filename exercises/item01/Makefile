CONTIKI_PROJECT = item01
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1

# DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_MAC=csma_driver
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver
CONTIKI = ../../main_contiki
include $(CONTIKI)/Makefile.include