CONTIKI_PROJECT = item04
TARGET = z1
all: $(CONTIKI_PROJECT)

CFLAGS += -DWITH_UIP=1

# Specify the RDC and MAC layers explicitly for Z1
DEFINES += NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

# Path to the main Contiki directory
CONTIKI = ../../main_contiki
include $(CONTIKI)/Makefile.include
