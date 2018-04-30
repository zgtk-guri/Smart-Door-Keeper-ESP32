#include "nfc.h"
#include <Arduino.h>
#include "port_def.h"
#include "PN532.h"
#include "PN532_SPI.h"

PN532_SPI pn532_spi(SPI, PORT_PN532_CS);
PN532 pn532(pn532_spi);

uint8_t prevIdm[8] = {0};
bool touching = false;
bool changed = false;

bool getCardTouching(){
    return touching;
}

uint8_t* getCardId(){
    return prevIdm;
}

bool isCardChanged(){
    if(changed){
        changed = false;
        return true;
    }
    return false;
}

void nfcReadTask(void *pvParameters){
    uint8_t result;
    uint16_t systemCode = 0xFFFF;
    uint8_t requestCode = 0x01;
    uint8_t idm[8];
    uint8_t pmm[8];
    uint16_t systemCodeResponse;

    digitalWrite(PORT_PN532_POW, HIGH);
    delay(1000);

    pn532.begin();
    pn532.setPassiveActivationRetries(0xFF);
    pn532.SAMConfig();
    
    
    while(1){
        result = pn532.felica_Polling(systemCode, requestCode, idm, pmm, &systemCodeResponse);
        if(result == 1){
            if(!touching || memcmp(idm, prevIdm, 8) != 0){
                memcpy(prevIdm, idm, 8);
                touching = true;
                changed = true;
            }
        }else{
            touching = false;
            changed = true;
        }
        delay(10);
    }
}