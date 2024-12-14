CONTIKI_PROJECT = item02
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1

# DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_MAC=csma_driver
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

CONTIKI = ../../main_contiki
CFLAGS += -I$(CONTIKI)/arch/platform/sky
CFLAGS += -I$(CONTIKI)/os
CFLAGS += -I/home/cbgarcia/contiki-ng/main_contiki/arch/dev/sensor/sht11
include $(CONTIKI)/Makefile.include
