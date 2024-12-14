CONTIKI_PROJECT = item03
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1

# Use nullrdc and csma as MAC and RDC
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

CONTIKI = ../../main_contiki
CFLAGS += -I$(CONTIKI)/arch/platform/sky
CFLAGS += -I$(CONTIKI)/os
CFLAGS += -I$(CONTIKI)/arch/dev/sensor/sht11
CFLAGS += -I$(CONTIKI)/os/net  # Ensure net directory is included

include $(CONTIKI)/Makefile.include