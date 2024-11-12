#include "contiki.h"
#include "dev/zoul-sensors.h"
#include "net/rime/rime.h"
#include <stdio.h>

#if !DEBUG
#include "dev/leds.h"
#endif

PROCESS(sink_proc, "base station process");
AUTOSTART_PROCESSES(&sink_proc);

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from){
	leds_toggle(LEDS_GREEN);
	printf("%i,%s,%i\n",from->u8[1],(char*)packetbuf_dataptr(),(int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));
	packetbuf_copyfrom("ACK",4);
	unicast_send(c, from);
}

static void
sent_uc(struct unicast_conn *c, int status, int num_tx){
	return;
}

static const struct unicast_callbacks ucc={recv_uc, sent_uc};
static struct unicast_conn uc;

PROCESS_THREAD(sink_proc, ev, data){
	PROCESS_EXITHANDLER(unicast_close(&uc));
	PROCESS_BEGIN();
	unicast_open(&uc, UNICAST_CHANNEL, &ucc);

	while(1){
		PROCESS_YIELD();
	}
	PROCESS_END();
}
