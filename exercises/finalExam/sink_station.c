#define UDP_PORT 1234

#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "dev/leds.h"
#include "sys/etimer.h"

static struct simple_udp_connection udp_connection;
static int count = 0;
static struct etimer no_data_timer;

struct temp_message {
    char type[5]; // Message type
    int value;    // Temperature
};

static void rcv_temp(struct simple_udp_connection *c,
                     const uip_ipaddr_t *sender_addr,
                     uint16_t sender_port,
                     const uip_ipaddr_t *receiver_addr,
                     uint16_t receiver_port,
                     const uint8_t *data,
                     uint16_t datalen) {
    
    etimer_restart(&no_data_timer);

    // Cast received data to the temp_message structure
    if (datalen == sizeof(struct temp_message)) {
        struct temp_message *temp_message = (struct temp_message *)data;
        printf("Temperature Sink: Received '%s' message with value: %d from sender\n", temp_message->type, temp_message->value);


        count++;

        // Control LEDs based on the counter value
        switch (count % 3) {
            case 0:
                leds_on(LEDS_RED);
                leds_off(LEDS_YELLOW | LEDS_GREEN);
                break;
            case 1:
                leds_on(LEDS_YELLOW);
                leds_off(LEDS_RED | LEDS_GREEN);
                break;
            case 2:
                leds_on(LEDS_GREEN);
                leds_off(LEDS_RED | LEDS_YELLOW);
                break;
        }

    }

    else {
        printf("Error: Received data size does not match expected temp_message size.\n");
    }
}

PROCESS(temp_sink_process, "Temperature Sink Process");
PROCESS_THREAD(temp_sink_process, ev, data) {
    PROCESS_BEGIN();

    // Register connection and recevive function
    simple_udp_register(&udp_connection, UDP_PORT, NULL, UDP_PORT, rcv_temp);
    printf("Temperature Sink process started. Listening on UDP port %d.\n", UDP_PORT);
    etimer_set(&no_data_timer, CLOCK_SECOND * 2);  // 2 seconds timeout

    while(1) {
        PROCESS_WAIT_EVENT();

        // Check if the no data timer has expired
        if (ev == PROCESS_EVENT_TIMER && etimer_expired(&no_data_timer)) {
            // Turn off led when no data received within 2 seconds
            leds_off(LEDS_RED | LEDS_YELLOW | LEDS_GREEN);
            count = 0;
            etimer_restart(&no_data_timer);
        }
    }

    PROCESS_END();
}

AUTOSTART_PROCESSES(&temp_sink_process);