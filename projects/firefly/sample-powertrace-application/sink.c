#include "contiki.h"
#include "dev/zoul-sensors.h"
#include "net/rime/rime.h"
#include <stdio.h>

#if DEBUG_APP
#include "powertrace.h"
#include "dev/leds.h"
#endif

PROCESS(sink_proc, "Base station process");
AUTOSTART_PROCESSES(&sink_proc);

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
#if DEBUG_APP
	leds_toggle(LEDS_GREEN);
	printf("Node %i: %s  [RSSI] %i\n", from->u8[1],(char*)packetbuf_dataptr(), (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));
#else
	printf("%i,%s,%i\n",from->u8[1],(char*)packetbuf_dataptr(),(int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));
#endif
	packetbuf_copyfrom("ACK", 4);
	unicast_send(c, from);
}

static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{
	const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
	if(linkaddr_cmp(dest, &linkaddr_null)){
		return;
	}
#if DEBUG_APP
	printf("Unicast ACK message sent to %d.%d [status %d num_tx %d]\n",dest->u8[0], dest->u8[1], status, num_tx);
#endif
}

static const struct unicast_callbacks ucc = {recv_uc, sent_uc};
static struct unicast_conn uc;

PROCESS_THREAD(sink_proc, ev, data)
{
	static struct etimer et;

	PROCESS_EXITHANDLER(unicast_close(&uc));

	PROCESS_BEGIN();
#if DEBUG_APP
	powertrace_start(CLOCK_SECOND * 10);
#endif
	unicast_open(&uc, UNICAST_CHANNEL, &ucc);
	etimer_set(&et, SAMPLING_INTERVAL);

	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		etimer_reset(&et);
	}
	PROCESS_END();
}
