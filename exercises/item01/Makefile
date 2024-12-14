CONTIKI_PROJECT = item01_1c item01_1d item01_1e item01_1f
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1
CFLAGS += -Wno-error=unused-result

# Define which RDC and MAC drivers to use
DEFINES = NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

CONTIKI = ../../main_contiki
include $(CONTIKI)/Makefile.include
