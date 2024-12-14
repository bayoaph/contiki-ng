TARGET = cooja
CONTIKI_PROJECT = finalExam
all: $(CONTIKI_PROJECT)

CFLAGS = -DWITH_UIP=1
CFLAGS += -Wno-error=unused-result

# Set the RDC and MAC drivers (you can modify these as needed)
DEFINES = NETSTACK_CONF_RDC=nullrdc_driver,NETSTACK_CONF_MAC=csma_driver

# Correct path to the Contiki-NG directory
CONTIKI = ../../main_contiki

include $(CONTIKI)/Makefile.include

SOURCES += source.c sink.c
