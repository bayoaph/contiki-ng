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
CONTIKI = /home/shaira/contiki-ng/main_contiki

# Include the Makefile from Contiki-NG
include $(CONTIKI)/Makefile.include

CFLAGS += -DUIP_CONF_IPV6=1
CFLAGS += -DUIP_CONF_UDP=1
CFLAGS += -DUIP_CONF_TCP=0
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
CFLAGS += -I$(CONTIKI)/os