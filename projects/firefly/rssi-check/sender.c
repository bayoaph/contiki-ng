#include "contiki.h"
#include "dev/zoul-sensors.h"
#include "net/rime/rime.h"
#include "sys/energest.h"
#include <stdio.h>
#include <stdlib.h>

static double econ;

#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#include "dev/leds.h"
#endif

PROCESS(main_proc, "RSSI sensing  process");
AUTOSTART_PROCESSES(&main_proc);

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
	PRINTF("[UC]Sink@%02x.%02x successfully received transmitted data!\n", (unsigned)from->u8[0],(unsigned)from->u8[1]);
#if !DEBUG
	leds_off(LEDS_ALL);
	leds_on(LEDS_GREEN);
#endif
}

static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{
	const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
	if(linkaddr_cmp(dest, &linkaddr_null))
		return;
	PRINTF("[UC]Unicast message sent to %02x.%02x [status %d num_tx %d]\n", (unsigned)dest->u8[0],(unsigned)dest->u8[1],status,num_tx);
#if !DEBUG
	leds_off(LEDS_ALL);
	leds_on(LEDS_RED);
#endif
	econ = 0;
}

static const struct unicast_callbacks ucc = {recv_uc, sent_uc};
static struct unicast_conn uc;

PROCESS_THREAD(main_proc, ev, data)
{
	static struct etimer et;
	char payload[80];
	static uint32_t batt;
	static double ecpu, elpm, etx, erx;
	static unsigned long pcpu, plpm, ptx, prx, cpu, lpm, tx, rx;
	linkaddr_t addr;

	PROCESS_EXITHANDLER(unicast_close(&uc));

	PROCESS_BEGIN();

	//initalize energest
	energest_init();
	ENERGEST_ON(ENERGEST_TYPE_CPU);
	ENERGEST_ON(ENERGEST_TYPE_LPM);
	ENERGEST_ON(ENERGEST_TYPE_TRANSMIT);
	ENERGEST_ON(ENERGEST_TYPE_LISTEN);
	energest_flush();
	pcpu = energest_type_time(ENERGEST_TYPE_CPU);
	plpm = energest_type_time(ENERGEST_TYPE_LPM);
	ptx = energest_type_time(ENERGEST_TYPE_TRANSMIT);
	prx = energest_type_time(ENERGEST_TYPE_LISTEN);
	econ = 0; ecpu = 0; elpm = 0; etx = 0; erx = 0;

	//initialize unicast communication
	unicast_open(&uc, UNICAST_CHANNEL, &ucc);

	etimer_set(&et, SAMPLING_INTERVAL);

	leds_off(LEDS_ALL);
	leds_on(LEDS_BLUE);

	PRINTF("[MAIN]process yielding...\n");
	PROCESS_YIELD();

	if(ev == PROCESS_EVENT_TIMER){
		PRINTF("[MAIN]timer callback\n");

		//get battery reading
		batt = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);

		//get latest energest readings
		energest_flush();
		cpu = energest_type_time(ENERGEST_TYPE_CPU); 
		lpm = energest_type_time(ENERGEST_TYPE_LPM);
		tx = energest_type_time(ENERGEST_TYPE_TRANSMIT);
		rx = energest_type_time(ENERGEST_TYPE_LISTEN);

		ecpu = (double)(cpu-pcpu)*FIREFLY_CPU_CURRENT*batt/(RTIMER_SECOND * (SAMPLING_INTERVAL/CLOCK_SECOND)*1.0);
		elpm = (double)(lpm-plpm)*FIREFLY_LPM_CURRENT*batt/(RTIMER_SECOND*(SAMPLING_INTERVAL/CLOCK_SECOND)*1.0);
		etx = (double)(tx-ptx)*FIREFLY_TX_CURRENT*batt/(RTIMER_SECOND*(SAMPLING_INTERVAL/CLOCK_SECOND)*1.0);
		erx = (double)(rx-prx)*FIREFLY_RX_CURRENT*batt/(RTIMER_SECOND*(SAMPLING_INTERVAL/CLOCK_SECOND)*1.0);
		econ = ecpu + elpm + etx + erx;

		sprintf(payload,"%04u,%04u,%04u,%04u,%04u,%04u",(unsigned)batt,(unsigned)ecpu,(unsigned)elpm,(unsigned)etx,(unsigned)erx,(unsigned)econ);

		packetbuf_copyfrom(payload, strlen(payload));
		addr.u8[0]=SINK_ADDRESS & 0xff00;
		addr.u8[1]=SINK_ADDRESS & 0x00ff;
		if(!linkaddr_cmp(&addr, &linkaddr_node_addr)){
			unicast_send(&uc, &addr);
			leds_off(LEDS_ALL);
		}
	}
	PROCESS_END();
}
