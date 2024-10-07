#include "contiki.h"
#include <stdio.h>
#include "net/rime/rime.h"

PROCESS(setup_proc, "Setup Process");
PROCESS(main_proc, "Main Process");
AUTOSTART_PROCESSES(&setup_proc);

static void recv_uc(struct unicast_conn *c, const linkaddr_t *from){
	printf("node%d:%s\n", from->u8[1], (char*)packetbuf_dataptr());
}

static void sent_uc(struct unicast_conn *c, int status, int num_tx){
}

static const struct unicast_callbacks ucc={recv_uc, sent_uc};
static struct unicast_conn uc;

PROCESS_THREAD(setup_proc, ev, data){
	PROCESS_EXITHANDLER(unicast_close(&uc));
	PROCESS_BEGIN();

	unicast_open(&uc, 146, &ucc);
	while(1){
		PROCESS_YIELD();
	}
	PROCESS_END();
}

PROCESS_THREAD(main_proc, ev, data){
	PROCESS_BEGIN();
	PROCESS_END();
}

