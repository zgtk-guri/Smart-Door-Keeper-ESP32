#include "util.h"
#include "Arduino.h"

uint32_t servoDegreeToDuty(double degree){
    if(degree < 0) degree = 0;
    if(degree > 180) degree = 180;
    return (uint32_t)(((degree / 180.0 ) * 0.0928 + 0.0272) * 65536.0);
}