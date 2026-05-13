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

Packet forwardPacket;
volatile bool hasForwardPacket = false;

void recvPacket(void) {
    greenLedToggle();
    Packet packet;
    int len = radioRecv(&packet, sizeof(packet));

    if (len != sizeof(packet)) {
        return;
    }

    int i;
    for (i = 0; i < sensorCount; i++) {
        if (sensorIds[i] == packet.src) {
            if (packet.seq > sensorSeqs[i]) {
                sensorSeqs[i] = packet.seq;
                packet.hops++;
                forwardPacket = packet;
                hasForwardPacket = true;
            }
            return;
        }
    }

    if (sensorCount < MAX_SENSORS) {
        sensorIds[sensorCount] = packet.src;
        sensorSeqs[sensorCount] = packet.seq;
        sensorCount++;
    }
    packet.hops++;
    forwardPacket = packet;
    hasForwardPacket = true;
}

void appMain(void) {
    radioSetReceiveHandle(recvPacket);
    radioOn();

    while (1) {
        if (hasForwardPacket) {
            hasForwardPacket = false;
            radioSend(&forwardPacket, sizeof(forwardPacket));
            redLedToggle();
        }
    }
}
