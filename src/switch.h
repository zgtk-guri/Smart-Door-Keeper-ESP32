#ifndef SWITCH_H
#define SWITCH_H

void initSwitches();
void switchPollingTask(void *pvParameters);

extern bool switches_stat[6];

inline bool getSw1IsPushed(){ return switches_stat[0]; };
inline bool getSw2IsPushed(){ return switches_stat[1]; };
inline bool getDipSw1IsOn(){ return switches_stat[2]; };
inline bool getDipSw2IsOn(){ return switches_stat[3]; };
inline bool getDoorIsClosed(){ return switches_stat[4]; };
inline bool getThumbTurnIsLocked(){ return switches_stat[5]; };

#endif