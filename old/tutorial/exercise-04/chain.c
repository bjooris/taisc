#include "../util/taisc.h"

void chain_radio_on(void){

    uint16_t start_time = 1000;

    onFromOff();
    stop(0);

}

void chain_send(void){

    uint16_t start_time = 2000;

    if(waitForTrigger(500, TAISC_EVENT_dataplane_txFrameAvailable)){

		loadFrame(0, 0);
        sync();
        tx();

    }

    stop(6000);

}
