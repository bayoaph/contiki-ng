#include "contiki.h"
#include "simple-udp.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>

#define UDP_PORT 1234

typedef struct __attribute__((__packed__)) {
    uint16_t temp_value; // Temperature in hundredths of a degree
} TempMsg;

static struct simple_udp_connection udp_connection;
static int led_state_index = 0;

static void update_led_state(int index) {
    switch (index % 3) {
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

static void display_temperature(const TempMsg *msg) {
    int16_t raw_temp = uip_ntohs(msg->temp_value);
    int whole_part = raw_temp / 100;
    int frac_part = raw_temp % 100;

    if (frac_part < 0) {
        frac_part = -frac_part;
    }

    printf("Received Temperature: %d.%02dC\n", whole_part, frac_part);
}

static void udp_message_handler(struct simple_udp_connection *conn,
                                 const uip_ipaddr_t *sender,
                                 uint16_t sender_port,
                                 const uip_ipaddr_t *receiver,
                                 uint16_t receiver_port,
                                 const uint8_t *data,
                                 uint16_t datalen) {
    if (datalen < sizeof(TempMsg)) {
        printf("Error: Message too short (received %d bytes)\n", datalen);
        return;
    }

    TempMsg received_msg;
    memcpy(&received_msg, data, sizeof(received_msg));

    led_state_index++;
    update_led_state(led_state_index);
    display_temperature(&received_msg);
}

PROCESS(sink_station_process, "Sink Station Process");
AUTOSTART_PROCESSES(&sink_station_process);

PROCESS_THREAD(sink_station_process, ev, data) {
    PROCESS_BEGIN();

    simple_udp_register(&udp_connection, UDP_PORT, NULL, UDP_PORT, udp_message_handler);
    printf("Sink Station active, awaiting messages on port %d\n", UDP_PORT);

    while (1) {
        PROCESS_WAIT_EVENT();
    }

    PROCESS_END();
}
