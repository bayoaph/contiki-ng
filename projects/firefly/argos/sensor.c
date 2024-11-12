#include "contiki.h"
#include "dev/zoul-sensors.h"
#include "net/rime/rime.h"
#include "net/rime/mesh.h"
#include "sys/energest.h"
#include "fog-mining.h"
#include "dev/leds.h"
#include <stdio.h>
#include <stdlib.h>

static double ecpu, elpm, etx, erx;

#if DEBUG_APP
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


PROCESS(sense_proc, "Temperature sensing process");
AUTOSTART_PROCESSES(&sense_proc);

void get_reading(fogdata_ptr val, linkaddr_t* addr, unsigned long* pcpu, unsigned long* plpm, unsigned long* ptx, unsigned long* prx, int forced);

//function that executes the L-SIP algorithm or similar smart transmission rules/protocols/algos
uint8_t data_check(fogdata_ptr d){
	//check for any value spikes
	if((d->prev-d->curr) > THRESHHOLD_DIFF || (d->curr-d->prev) > THRESHHOLD_DIFF){
		d->prev = d->curr;
		d->safelen++;	//count as 15mins has definitely elapsed
		return 1;
	}else if(d->safelen >= MIN_INVOLUNTARY_INTERVAL){ //if 1 hour is up
		d->safelen = 0; //reset counter
		return 1;
	}else //15mins has elapsed but value is not too different than the previous reading
		d->safelen += 1;
	return 0;
}

static void
recv_mc(struct mesh_conn *c, const linkaddr_t *from, uint8_t hops)
{
	PRINTF("[UC]Sink@%d.%d successfully received transmitted data!\n",from->u8[0],from->u8[1]);
	leds_off(LEDS_WHITE);
}

static void
sent_mc(struct mesh_conn *c)
{
	PRINTF("Packet sent!\n");
	leds_off(LEDS_WHITE);
	leds_on(LEDS_RED);
}

static void
timedout(struct mesh_conn *c){
	PRINTF("Transmission timed out.\n");
	leds_off(LEDS_WHITE);
	leds_on(LEDS_BLUE);
}

const static struct mesh_callbacks mcc = {recv_mc, sent_mc, timedout};
static struct mesh_conn mesh;

PROCESS_THREAD(sense_proc, ev, data)
{
	static struct etimer et;
	static unsigned long  pcpu, plpm, ptx, prx;
	static fogdata_t val;
	linkaddr_t addr;

	PROCESS_EXITHANDLER(mesh_close(&mesh));

	PROCESS_BEGIN();
	//initialize energest and get initial energy readings
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
	ecpu = 0; elpm = 0; etx = 0; erx = 0;

	//initialize unicast communication
	mesh_open(&mesh, MESH_CHANNEL, &mcc);

	//set initial temp reading to zero to make sure first transmission happens
	val.prev = 0;
	val.safelen = 0;

	//initialize timer (15mins)
	etimer_set(&et, SAMPLING_INTERVAL);

	while(1){
		PRINTF("[SENSOR] process yielding...\n");
		PROCESS_YIELD();

		leds_off(LEDS_WHITE);
		if((ev == sensors_event)&&(data == &button_sensor)){
			if(button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL)==BUTTON_SENSOR_PRESSED_LEVEL){
				PRINTF("[SENSOR] get current reading\n");
				get_reading(&val, &addr, &pcpu, &plpm, &ptx, &prx, FORCED_READING);
			}else
				PRINTF("[SENSOR] button released!\n");
		}

		if(ev == PROCESS_EVENT_TIMER){
			PRINTF("[SENSOR] timer callback\n");
			get_reading(&val, &addr, &pcpu, &plpm, &ptx, &prx, !FORCED_READING);
			etimer_reset(&et);
		}
	}
	PROCESS_END();
}

void get_reading(fogdata_ptr val, linkaddr_t* addr, unsigned long* pcpu, unsigned long* plpm, unsigned long* ptx, unsigned long* prx, int forced){
	char payload[53];
	static uint16_t temp;
	static uint32_t batt;
	static double ccpu, clpm, ctx, crx;
	static unsigned long  cpu, lpm, tx, rx;

	//get battery reading
	batt = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);

	//get latest energest readings
	energest_flush();
	cpu = energest_type_time(ENERGEST_TYPE_CPU);
	lpm = energest_type_time(ENERGEST_TYPE_LPM);
	tx = energest_type_time(ENERGEST_TYPE_TRANSMIT);
	rx = energest_type_time(ENERGEST_TYPE_LISTEN);

	//compute energy consumption
	ccpu = (double)(cpu-*pcpu)*FIREFLY_CPU_CURRENT*batt/(RTIMER_SECOND*(SAMPLING_INTERVAL/CLOCK_SECOND)*1.0);
	clpm = (double)(lpm-*plpm)*FIREFLY_LPM_CURRENT*batt/(RTIMER_SECOND*(SAMPLING_INTERVAL/CLOCK_SECOND)*1.0);
	ctx = (double)(tx-*ptx)*FIREFLY_TX_CURRENT*batt/(RTIMER_SECOND*(SAMPLING_INTERVAL/CLOCK_SECOND*1.0));
	crx = (double)(rx-*prx)*FIREFLY_RX_CURRENT*batt/(RTIMER_SECOND*(SAMPLING_INTERVAL/CLOCK_SECOND)*1.0);
	ecpu += ccpu;
	elpm += clpm;
	etx += ctx;
	erx += crx;

	PRINTF("[SENSOR] cpu: %lu, pcpu: %lu, Rsec: %lu,", cpu, *pcpu, (long unsigned)RTIMER_SECOND);
	PRINTF("tl:%lu, ccpu: %lu, ", (long unsigned)SAMPLING_INTERVAL/CLOCK_SECOND, (unsigned long)ccpu);
	PRINTF("ecpu: %lu.%u uW\n", (long unsigned)(ecpu), (unsigned)(ecpu*100)%100);
	PRINTF("[SENSOR] lpm: %lu, elpm: %lu.%u uW\n", lpm, (long unsigned)(elpm), (unsigned)(elpm*100)%100);
	PRINTF("[SENSOR] tx: %lu, ptx: %lu, ctx: %lu, ", tx, *ptx, (long unsigned)ctx);
	PRINTF("etx: %lu.%u uW\n",(long unsigned)(etx), (unsigned)(etx*100)%100);
	PRINTF("[SENSOR] rx: %lu, erx: %lu.%u uW\n", rx, (long unsigned)(erx), (unsigned)(erx*100)%100);

	*pcpu = cpu;
	*plpm = lpm;
	*ptx = tx;
	*prx = rx;

	temp = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
	val->curr = temp;

	PRINTF("[SENSOR] batt: %u.%uV, temp: %u.%uC\n",(unsigned)batt/1000, (unsigned)batt%1000, (unsigned)temp/1000, (unsigned)temp%1000);

	//if battery is critical turn on red LED
	if(batt <= 2000){
	}

	//if temperature spikes out, transmit data
	PRINTF("[SENSOR] addr:%02x:%02x\n", (unsigned)SINK_ADDRESS&0xff00,(unsigned)SINK_ADDRESS&0x00ff);
	if(data_check(val) || forced){
		//format data
		sprintf(payload,"%+06ld,%+07d,%04u.%02u,%04u.%02u,%06u.%02u,%06u.%02u",batt,temp,(unsigned)ecpu,(unsigned)(ecpu*100.0)%100u,(unsigned)elpm,(unsigned)(elpm*100.0)%100u,(unsigned)etx,(unsigned)(etx*100.0)%100u,(unsigned)erx,(unsigned)(erx*100.0)%100u);
		PRINTF("[SENSOR] payload: '%s'\n", payload);

		//transmit data
		packetbuf_copyfrom(payload, strlen(payload));
		addr->u8[0] = SINK_ADDRESS & 0xff00;
		addr->u8[1] = SINK_ADDRESS & 0x00ff;
		if(!linkaddr_cmp(addr, &linkaddr_node_addr)){
			mesh_send(&mesh, addr);
			leds_off(LEDS_BLUE);
			leds_on(LEDS_GREEN);
		}
	}
}

