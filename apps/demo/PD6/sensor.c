#include "stdmansos.h"

typedef struct {
    uint16_t src;
    uint8_t seq;
    uint8_t hops;
    uint16_t light;
} Packet;

void appMain(void) {
    uint8_t seq = 0;
    radioOn();

    while (1) {
        Packet packet;
        packet.src = localAddress;
        packet.seq = seq;
        packet.hops = 0;
        packet.light = lightRead();

        radioSend(&packet, sizeof(packet));
        seq++;

        redLedToggle();
        mdelay(3000);
    }
}
