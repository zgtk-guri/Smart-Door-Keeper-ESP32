#include "switch.h"
#include <Arduino.h>
#include "port_def.h"

bool switches_stat[6] = {false};

void initSwitches(){
    pinMode(PORT_SW1, INPUT_PULLUP);
    pinMode(PORT_SW2, INPUT_PULLUP);
    pinMode(PORT_DIPSW1, INPUT_PULLUP);
    pinMode(PORT_DIPSW2, INPUT_PULLUP);
    pinMode(PORT_DOOR_SW, INPUT_PULLUP);
    pinMode(PORT_LOCK_DET, INPUT_PULLUP);
}

void switchPollingTask(void *pvParameters){
    const uint8_t ports[6] = {PORT_SW1, PORT_SW2, PORT_DIPSW1, PORT_DIPSW2, PORT_DOOR_SW, PORT_LOCK_DET};
    while(1){
        for(int i = 0; i < 6; i++){
            switches_stat[i] = digitalRead(ports[i]) == LOW;
        }
        delay(50);
    }
}