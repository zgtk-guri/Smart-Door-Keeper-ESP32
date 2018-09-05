#include "switch.h"
#include <Arduino.h>
#include "port_def.h"

bool switches_stat[6] = {false};
bool switches_clicked[2] = {false};

void initSwitches(){
    pinMode(PORT_SW1, INPUT_PULLUP);
    pinMode(PORT_SW2, INPUT_PULLUP);
    pinMode(PORT_DIPSW1, INPUT_PULLUP);
    pinMode(PORT_DIPSW2, INPUT_PULLUP);
    pinMode(PORT_DOOR_SW, INPUT_PULLUP);
    pinMode(PORT_LOCK_DET, INPUT_PULLUP);
}

bool getSw1IsPushed(){ return switches_stat[0]; };
bool getSw2IsPushed(){ return switches_stat[1]; };
bool getDipSw1IsOn(){ return switches_stat[2]; };
bool getDipSw2IsOn(){ return switches_stat[3]; };
bool getDoorIsClosed(){ return switches_stat[4]; };
bool getThumbTurnIsLocked(){ return !switches_stat[5]; };

void switchPollingTask(void *pvParameters){
    const uint8_t ports[6] = {PORT_SW1, PORT_SW2, PORT_DIPSW1, PORT_DIPSW2, PORT_DOOR_SW, PORT_LOCK_DET};
    bool prev_switches_stat[6] = {false};
    while(1){
        for(int i = 0; i < 6; i++){
            switches_stat[i] = digitalRead(ports[i]) == LOW;
            if(i < 2 && switches_stat[i] != prev_switches_stat[i] && switches_stat[i]){
                switches_clicked[i] = true;
            }
            prev_switches_stat[i] = switches_stat[i];
        }
        delay(50);
    }
}

bool isSw1Clicked(){
    if(switches_clicked[0]){
        switches_clicked[0] = false;
        return true;
    }
    return false;
}

bool isSw2Clicked(){
    if(switches_clicked[1]){
        switches_clicked[1] = false;
        return true;
    }
    return false;
}