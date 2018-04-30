#include "util.h"
#include "Arduino.h"

uint32_t servoDegreeToDuty(double degree){
    if(degree < 0) degree = 0;
    if(degree > 180) degree = 180;
    return (uint32_t)(((degree / 180.0 ) * 0.0928 + 0.0272) * 65536.0);
}

String idmToString(uint8_t *idm){
    String str = "";
    for(int i = 0; i < 8; i++){
        str += (idm[i] < 0x10 ? "0" : "") + String(idm[i], 16);
    }
    return str;
}
