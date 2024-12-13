CONTIKI_PROJECT = item01
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1
CFLAGS += -Wno-error=unused-result

# Set RDC and MAC layer drivers
DEFINES = NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

# Corrected Contiki path for WSL
CONTIKI = /home/jlmejia/IoT/contiki-ng/_setup/main_contiki
include $(CONTIKI)/Makefile.include
