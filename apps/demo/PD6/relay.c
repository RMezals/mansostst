#include "stdmansos.h"

typedef struct {
    uint16_t src;
    uint8_t seq;
    uint16_t light;
} Packet;

#define MAX_SENSORS 8
uint16_t sensorIds[MAX_SENSORS];
uint8_t sensorSeqs[MAX_SENSORS];
int sensorCount = 0;

void recvPacket(void) {
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
                radioSend(&packet, sizeof(packet));
                greenLedToggle();
            }
            return;
        }
    }

    if (sensorCount < MAX_SENSORS) {
        sensorIds[sensorCount] = packet.src;
        sensorSeqs[sensorCount] = packet.seq;
        sensorCount++;
    }
    radioSend(&packet, sizeof(packet));
    greenLedToggle();
}

void appMain(void) {
    radioSetReceiveHandle(recvPacket);
    radioOn();
}
