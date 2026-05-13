#include "stdmansos.h"

typedef struct {
    uint16_t src;
    uint8_t  seq;
    uint16_t light;
} __attribute__((packed)) Packet;

void appMain(void) {
    radioOn();
    uint8_t seq = 0;
    for (;;) {
        Packet pkt = {localAddress, seq++, lightRead()};
        radioSend(&pkt, sizeof(pkt));
        redLedToggle();
        mdelay(3000);
    }
}
