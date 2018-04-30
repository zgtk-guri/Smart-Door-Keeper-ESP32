#ifndef BEEP_H
#define BEEP_H

const int BEEP_HIGH_FREQ = 1000;
const int BEEP_LOW_FREQ = 700;

const int BEEP_LONG_LEN = 10;

void beep1Short(int freq);
void beep1Long(int freq);
void beep2ShortShort(int freq1, int freq2);
void beep2ShortLong(int freq1, int freq2);

void initBeep();

void beeptask(void *pvParameters);

#endif