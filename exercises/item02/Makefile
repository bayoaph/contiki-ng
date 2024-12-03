CONTIKI_PROJECT = item02
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1
CFLAGS += -Wno-error=unused-result

# DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_MAC=csma_driver
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

CONTIKI = /home/kcerio/IoT/contiki-ng/_setup/main_contiki
include $(CONTIKI)/Makefile.include
CFLAGS += -I$(CONTIKI)/arch/platform/zoul
CFLAGS += -I$(CONTIKI)/os