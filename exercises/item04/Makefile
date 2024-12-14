CONTIKI_PROJECT = item03
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1
CFLAGS += -Wno-error=unused-result
PROJECT_SOURCEFILES += sht25.c

# DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_MAC=csma_driver
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

CONTIKI = ../../main_contiki
CFLAGS += -I$(CONTIKI)/os
include $(CONTIKI)/Makefile.include