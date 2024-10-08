#include "contiki.h"
#include "dev/zoul-sensors.h"
#include "dev/leds.h"
#include "net/rime/rime.h"
#include "net/rime/mesh.h"
#include <stdio.h>

#if DEBUG_APP
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

PROCESS(sink_proc, "Base station process");
AUTOSTART_PROCESSES(&sink_proc);

static void
recv_mc(struct mesh_conn *c, const linkaddr_t *from, uint8_t hops)
{
	leds_off(LEDS_WHITE);
	leds_on(LEDS_GREEN);

	//send data serially to rpi
	printf("%i,%s,%i,%i,%i\n",from->u8[1],(char*)packetbuf_dataptr(),(int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI),(int16_t)packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY), hops);
	//reply to sender with acknowledgement
	packetbuf_copyfrom("ACK", 4);
	mesh_send(c, from);
}

static void
timedout(struct mesh_conn *c){
	PRINTF("Transmission timedout!\n");
	leds_off(LEDS_WHITE);
}

static void
sent_mc(struct mesh_conn *c)
{
	const linkaddr_t *dest = packetbuf_addr(PACKETBUF_ADDR_RECEIVER);
	if(linkaddr_cmp(dest, &linkaddr_null)){
		return;
	}
#if DEBUG_APP
	printf("Unicast ACK message sent to %d.%d\n",dest->u8[0], dest->u8[1]);
#endif
	leds_off(LEDS_WHITE);
}

const static struct mesh_callbacks mcc = {recv_mc, sent_mc, timedout};
static struct mesh_conn mesh;

PROCESS_THREAD(sink_proc, ev, data)
{

	PROCESS_EXITHANDLER(mesh_close(&mesh));

	PROCESS_BEGIN();

	mesh_open(&mesh, MESH_CHANNEL, &mcc);

	while(1){
		PRINTF("[SINK]Process yielding...\n");
		PROCESS_YIELD();
	}
	PROCESS_END();
}
