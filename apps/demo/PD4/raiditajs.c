#include "stdmansos.h"

typedef struct {
	char key[4];
	uint8_t value;
} Msg;


void appMain(void)
{

    radioOn();

    while (1) {
    	
    	uint8_t light = lightRead();
    	PRINTF("gaisma: %i\n", light);
        Msg zina = {"iza", light};
    
        radioSend(&zina, sizeof(zina));
        redLedToggle();
        mdelay(3000);
    }
}
