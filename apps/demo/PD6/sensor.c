#include "stdmansos.h"
#include <string.h>

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

        Msg zina;

        memset(&zina, 0, sizeof(zina));

        memcpy(zina.key, "iza", 4);  
        zina.value = light;

        radioSend(&zina, sizeof(zina));

        redLedToggle();
        mdelay(3000);
    }
}
