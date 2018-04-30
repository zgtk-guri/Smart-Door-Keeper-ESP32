#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>

//degree: 0-180
uint32_t servoDegreeToDuty(double degree);

class String;
String idmToString(uint8_t *idm);

#endif