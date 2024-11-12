#include "contiki.h"
#include "dev/zoul-sensors.h"
#include "dev/lcd.h"
#include "net/rime/rime.h"
#include "fog-mining.h"
#include <stdio.h>
#include <stdlib.h>

#if DEBUG_APP
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

PROCESS(setup_proc, "Meter setup process");	/* process that setups the node */
PROCESS(loop_proc, "Meter curtail process");	/* process that serves as the main loop */
AUTOSTART_PROCESSES(&setup_proc);

void send_data(char* payload, linkaddr_t* addr);

//function that executes the L-SIP algorithm or similar smart transmission rules/protocols/algos
/*uint8_t data_check(fogdata_ptr d){
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
}*/

// When node receives a unicast message (typically when new balance is added to the meter), run rld_proc
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
	PRINTF("[UC]Sink@%d.%d successfully received transmitted data!\n",from->u8[0],from->u8[1]);
}

// When data is sent, check if other proc needs priority else sleep
static void
sent_uc(struct unicast_conn *c, int status, int num_tx)
{
	const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
	if(linkaddr_cmp(dest, &linkaddr_null)){
		return;
	}
	PRINTF("[UC]Unicast message sent to %d.%d [status %d num_tx %d]\n",dest->u8[0], dest->u8[1], status, num_tx);
}

static const struct unicast_callbacks ucc = {recv_uc, sent_uc};
static struct unicast_conn uc;

PROCESS_THREAD(setup_proc, ev, data)
{
	static fogdata_t val;

	PROCESS_EXITHANDLER(unicast_close(&uc));

	PROCESS_BEGIN();

	//setup meter trigger pin
	#define METER_REQ_PORT	GPIO_PORT_TO_BASE(GPIO_A_NUM)
	#define METER_REQ_PIN	(1<<5)

	GPIO_SOFTWARE_CONTROL(METER_REQ_PORT, METER_REQ_PIN);
	GPIO_SET_OUTPUT(METER_REQ_PORT, METER_REQ_PIN);
	GPIO_CLR_PIN(METER_REQ_PORT, METER_REQ_PIN);
	PRINTF("[SETUP] PA5 configured!\n");

	//configure lcd
	SENSORS_ACTIVATE(rgb_bl_lcd);
	PRINTF("[SETUP] LCD configured!\n");

	//initialize unicast communication
	unicast_open(&uc, UNICAST_CHANNEL, &ucc);

	//set initial temp reading to zero to make sure first transmission happens
	val.prev = 0;
	val.safelen = 0;

	process_start(&loop_proc,&val);

	PROCESS_END();
}

PROCESS_THREAD(loop_proc, ev, data){
	linkaddr_t addr;
	static struct etimer et;
	static fogdata_t val;
	static int count = 0;
	char dtxt[17];

	PROCESS_BEGIN();

	val = (fogdata_t) val;

	while(1){
		etimer_set(&et, 5*CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		count++;
		PRINTF("%03d\n",count);
		sprintf(dtxt,"#%03d",count);
		lcd_clear_display();
                lcd_set_cursor(1,0);
		lcd_write(dtxt);
		send_data("test", &addr);
		etimer_reset(&et);
	}
	PROCESS_END();
}

void send_data(char* payload, linkaddr_t* addr){

	packetbuf_copyfrom(payload,strlen(payload));
	addr->u8[0] = 0x0005 & 0xff00;
	addr->u8[1] = 0x0005 & 0x00ff;
	if(!linkaddr_cmp(addr, &linkaddr_node_addr)){
		unicast_send(&uc, addr);
	}
}

