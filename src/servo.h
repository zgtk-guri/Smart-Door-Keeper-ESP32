#ifndef SERVO_H
#define SERVO_H

void servoInit();
void servoLock();
void servoUnlock();
void servoRelease();
bool getServoPositionIsLocked();
void setServoPositions(int locked, int unlocked);

#endif