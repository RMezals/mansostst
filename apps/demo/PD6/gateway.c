#include "stdmansos.h"

typedef struct {
    uint16_t src;
    uint8_t seq;
    uint8_t hops;
    uint16_t light;
} Packet;

#define MAX_SENSORS 8
uint16_t sensorIds[MAX_SENSORS];
uint8_t sensorSeqs[MAX_SENSORS];
int sensorCount = 0;

void recvPacket(void) {
    blueLedToggle();
    Packet packet;
    int len = radioRecv(&packet, sizeof(packet));

    if (len != sizeof(packet)) {
        return;
    }

    if (packet.hops == 0) {
        return;
    }

    int i;
    for (i = 0; i < sensorCount; i++) {
        if (sensorIds[i] == packet.src) {
            if (packet.seq > sensorSeqs[i]) {
                sensorSeqs[i] = packet.seq;
                PRINTF("src=%u seq=%u hops=%u light=%u\n", packet.src, packet.seq, packet.hops, packet.light);
                redLedToggle();
            }
            return;
        }
    }

    if (sensorCount < MAX_SENSORS) {
        sensorIds[sensorCount] = packet.src;
        sensorSeqs[sensorCount] = packet.seq;
        sensorCount++;
    }
    PRINTF("src=%u seq=%u hops=%u light=%u\n", packet.src, packet.seq, packet.hops, packet.light);
    redLedToggle();
}

void appMain(void) {
    radioSetReceiveHandle(recvPacket);
    radioOn();
}
