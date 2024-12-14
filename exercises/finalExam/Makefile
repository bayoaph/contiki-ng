CONTIKI_PROJECT = finalexam
all: $(CONTIKI_PROJECT)

# Compiler flags
CFLAGS += -DWITH_UIP=1
CFLAGS += -Wno-error=unused-result
CFLAGS += -Wno-error

# Define the network stack
DEFINES += NETSTACK_CONF_RDC=nullrdc_driver
DEFINES += NETSTACK_CONF_MAC=csma_driver

# Path to Contiki-NG (adjust if necessary)
CONTIKI = /home/kcerio/IoT/contiki-ng/_setup/main_contiki

# Include the Makefile from Contiki-NG
include $(CONTIKI)/Makefile.include
CFLAGS += -I$(CONTIKI)/os