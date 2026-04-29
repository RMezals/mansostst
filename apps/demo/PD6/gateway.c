#include "stdmansos.h"

typedef struct {
	char key[4];
	uint8_t value;
} Msg;

void recvRadio(void) {
	Msg sanemt = {"", 0};

	radioRecv(&sanemt, sizeof(sanemt));

	if(strncmp(sanemt.key, "iza", strlen("iza")) == 0 ) {
		PRINTF("%i\n", sanemt.value);
	}
}

void appMain(void) {
	radioSetReceiveHandle(recvRadio);
	radioOn();
}