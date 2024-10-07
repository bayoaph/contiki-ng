CONTIKI_PROJECT = sensor sink
CONTIKI_TARGET_SOURCEFILES += rgb-bl-lcd.c

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
CFLAGS += -DLCD_ENABLED=1
CONTIKI_WITH_RIME = 1

all: $(CONTIKI_PROJECT)
CONTIKI=../../..
include $(CONTIKI)/Makefile.include
