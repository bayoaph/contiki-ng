#include "contiki.h"
#include "dev/zoul-sensors.h"
#include "net/rime/rime.h"
#include "fog-mining.h"
#include <stdio.h>

#if DEBUG_APP
#include "powertrace.h"
#endif

PROCESS(sense_proc, "Temperature sensing process");
AUTOSTART_PROCESSES(&sense_proc);

//function that executes the L-SIP algorithm or what not
uint8_t data_check(fogdata_ptr d){
	if((d->prev-d->curr) > THRESHHOLD_DIFF || (d->curr-d->prev) > THRESHHOLD_DIFF){
		d->prev = d->curr;
		return 1;
	}else if(d->safelen > 59){
		d->safelen = 0;
		return 1;
	}else
		d->safelen += 1;
	return 0;
}

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
#if DEBUG_APP
#else
	printf("Sink@%d.%d successfully received transmitted data!\n",from->u8[0],from->u8[1]);
#endif
}

static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{
	const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
	if(linkaddr_cmp(dest, &linkaddr_null)){
		return;
	}
#if DEBUG_APP
#else
	printf("Unicast message sent to %d.%d [status %d num_tx %d]\n",dest->u8[0], dest->u8[1], status, num_tx);
#endif
}

static const struct unicast_callbacks ucc = {recv_uc, sent_uc};
static struct unicast_conn uc;

PROCESS_THREAD(sense_proc, ev, data)
{
	static struct etimer et;
	char payload[10];
	static uint16_t temp;
	static uint32_t batt;
	static fogdata_t val;
	linkaddr_t addr;

	PROCESS_EXITHANDLER(unicast_close(&uc));

	PROCESS_BEGIN();
#if DEBUG_APP
	powertrace_start(10*CLOCK_SECOND);
#endif
	unicast_open(&uc, UNICAST_CHANNEL, &ucc);
	val.prev = 0; val.safelen = 0;
	etimer_set(&et, SAMPLING_INTERVAL);

	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		batt = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
		temp = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);

		val.curr = temp;
		//if temperature spikes out, transmit data
		if(data_check(&val)){
			sprintf(payload, "%4u,%4u",(unsigned)batt,(unsigned)temp);
			packetbuf_copyfrom(payload, strlen(payload));
			addr.u8[0] = 0;
			addr.u8[1] = 255;
			if(!linkaddr_cmp(&addr, &linkaddr_node_addr))
				unicast_send(&uc, &addr);
		}
		etimer_reset(&et);
	}
	PROCESS_END();
}
