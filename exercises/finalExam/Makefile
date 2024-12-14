CONTIKI_PROJECT = exercise5
TARGET=sky
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1
CFLAGS += -DUIP_CONF_IPV6=1  # Ensure IPv6 is enabled

# DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_MAC=csma_driver
# DEFINES=NETSTACK_CONF_RDC=xmac_driver,NETSTACK_CONF_MAC=csma_driver
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

CONTIKI = ../../main_contiki
CFLAGS += -I$(CONTIKI)/arch/platform/sky
CFLAGS += -I$(CONTIKI)/os
CFLAGS += -I$(CONTIKI)/arch/dev/sensor/sht11
CFLAGS += -I$(CONTIKI)/os/net  # Ensure net directory is included

CFLAGS += -I$(CONTIKI)/os/net/ipv6/simple-udp

include $(CONTIKI)/Makefile.include