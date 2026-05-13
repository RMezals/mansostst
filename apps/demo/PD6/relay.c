#include "stdmansos.h"

typedef struct {
    uint16_t src;
    uint8_t  seq;
    uint16_t light;
} __attribute__((packed)) Packet;

#define MAX_SOURCES 8
static uint16_t knownSrc[MAX_SOURCES];
static uint8_t  lastSeq[MAX_SOURCES];
static uint8_t  srcCount = 0;

static bool shouldForward(uint16_t src, uint8_t seq) {
    uint8_t i;
    for (i = 0; i < srcCount; i++) {
        if (knownSrc[i] != src) continue;
        if ((int8_t)(seq - lastSeq[i]) <= 0) return false;
        lastSeq[i] = seq;
        return true;
    }
    if (srcCount < MAX_SOURCES) {
        knownSrc[srcCount] = src;
        lastSeq[srcCount]  = seq;
        srcCount++;
    }
    return true;
}

static void onRecv(void) {
    Packet pkt;
    if (radioRecv(&pkt, sizeof(pkt)) != sizeof(pkt)) return;
    if (!shouldForward(pkt.src, pkt.seq)) return;
    greenLedToggle();
    radioSend(&pkt, sizeof(pkt));
}

void appMain(void) {
    radioSetReceiveHandle(onRecv);
    radioOn();
}
