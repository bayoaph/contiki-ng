CONTIKI_PROJECT = source sink
all: $(CONTIKI_PROJECT)

CONTIKI = ../../main_contiki
PROJECT_SOURCEFILES += sht25.c

MODULES += os/services/coap os/services/simple-udp os/net
MODULES_REL += arch/platform/z1/dev/sht25.c 

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

include $(CONTIKI)/Makefile.include
