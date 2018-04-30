#include "servo.h"
#include <Arduino.h>
#include "util.h"
#include "port_def.h"

int servoLockPosition = 90, servoUnlockPosition = 90;
bool servoLocked = false;

void servoInit(){
    digitalWrite(PORT_SERVO_POW, LOW);
    pinMode(PORT_SERVO_POW, OUTPUT);
    digitalWrite(PORT_SERVO_SIG, LOW);
    pinMode(PORT_SERVO_SIG, OUTPUT);
    ledcSetup(SERVO_LEDC_CHANNEL, 50, 16);
    ledcAttachPin(PORT_SERVO_SIG, SERVO_LEDC_CHANNEL);
}

void servoLock(){
    digitalWrite(PORT_SERVO_POW, HIGH);
    ledcWrite(SERVO_LEDC_CHANNEL, servoDegreeToDuty(servoLockPosition));
    servoLocked = true;
}

void servoUnlock(){
    digitalWrite(PORT_SERVO_POW, HIGH);
    ledcWrite(SERVO_LEDC_CHANNEL, servoDegreeToDuty(servoUnlockPosition));
    servoLocked = false;
}

void servoRelease(){
    digitalWrite(PORT_SERVO_POW, LOW);
}

bool getServoPositionIsLocked(){
    return servoLocked;
}

void setServoPositions(int locked, int unlocked){
    servoLockPosition = locked;
    servoUnlockPosition = unlocked;
}