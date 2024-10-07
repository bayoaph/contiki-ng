#include "contiki.h"
#include <stdio.h>
#include "dev/msc/bh1750.h"
#include "net/rime/rime.h"
#include "project-conf.h"

#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__);
#else
#define PRINTF(...)
#endif

PROCESS(main_proc, "Main Process");
PROCESS(setup_proc, "Setup Process");
AUTOSTART_PROCESSES(&setup_proc);

static void recv_uc(struct unicast_conn *c, const linkaddr_t *from){
	PRINTF("recv_uc: received message from another mote");
}

static void sent_uc(struct unicast_conn *c, int status, int num_tx){
	const linkaddr_t *dest = packetbuf_addr (PACKETBUF_ADDR_RECEIVER);
	if(linkaddr_cmp(dest, &linkaddr_null)){
		return;
	}
	printf("unicast message sent to %d.%d: status %d num_tx %d\n",dest->u8[0], dest->u8[1], status, num_tx);
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc, sent_uc};
static struct unicast_conn uc;

PROCESS_THREAD(setup_proc, ev, data){
	PROCESS_BEGIN();
	PRINTF("setup: initialised\n");
	//setup network

	//then start sensing
	process_start(&main_proc, NULL);
	PROCESS_END();
}

PROCESS_THREAD(main_proc, ev, data){
	static struct etimer et;
	static int val;//, bat;
	char payload[15];
	linkaddr_t addr;

	PROCESS_EXITHANDLER(unicast_close(&uc));

	PROCESS_BEGIN();

	PRINTF("main: Initialised\n");
//	SENSORS_ACTIVATE(bh1750);
	unicast_open(&uc, 146, &unicast_callbacks);
	SENSORS_ACTIVATE(bh1750);
	etimer_set(&et, 3*CLOCK_SECOND);

	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		val = bh1750.value(CONT_LRES);
		//bat = vdd3_sensor.value(0);

		sprintf(payload, "%5ilx", val);
	//	PRINTF("Main Proc: batt=%i.%02iV light=%i lx\n", bat/10000, bat%10000, val);

		packetbuf_copyfrom(payload, sizeof(payload));
		addr.u8[0]=0;
		addr.u8[1]=1;
		unicast_send(&uc, &addr);

		etimer_reset(&et);

	}

	PROCESS_END();
}
