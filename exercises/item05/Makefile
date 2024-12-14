CONTIKI_PROJECT = item05
TARGET=z1
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1

# Define the RDC and MAC layers. Z1 supports the same configurations as Sky.
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

# Update the Contiki path to ../../main_contiki
CONTIKI = ../../main_contiki
include $(CONTIKI)/Makefile.include
