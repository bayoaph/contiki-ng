CONTIKI_PROJECT = exercise5
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1
CFLAGS += -Wno-error=unused-result

# DEFINES=NETSTACK_CONF_RDC=contikimac_driver,NETSTACK_CONF_MAC=csma_driver
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

APPS += csma
CONTIKI = ../../main_contiki
# CFLAGS += -I$(CONTIKI)/arch/platform/zoul
CFLAGS += -I$(CONTIKI)/os
# CFLAGS += -I/Users/liaminakigillamac/ti/msp430-gcc/include
# LDFLAGS += -L/Users/liaminakigillamac/ti/msp430-gcc/include/
# LDFLAGS += -T/Users/liaminakigillamac/ti/msp430-gcc/include/msp430f1611.ld
# LDFLAGS += -T/Users/liaminakigillamac/ti/msp430-gcc/include/msp430f1611_symbols.ld

include $(CONTIKI)/Makefile.include