#ifndef SWITCH_H
#define SWITCH_H

void initSwitches();
void switchPollingTask(void *pvParameters);

bool getSw1IsPushed();
bool getSw2IsPushed();
bool getDipSw1IsOn();
bool getDipSw2IsOn();
bool getDoorIsClosed();
bool getThumbTurnIsLocked();

bool isSw1Clicked();
bool isSw2Clicked();

#endif