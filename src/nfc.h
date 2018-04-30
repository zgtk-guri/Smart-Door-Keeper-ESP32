#ifndef NFC_H
#define NFC_H
#include <stdint.h>

bool getCardTouching();
uint8_t* getCardId();
bool isCardChanged();
void nfcReadTask(void *pvParameters);

#endif