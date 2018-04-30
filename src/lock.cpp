#include "lock.h"
#include <Arduino.h>
#include "switch.h"
#include "servo.h"

bool lockLocked = true;
int lockServoReleaseTimeout = -1;

void lock(){
    servoLock();
    lockServoReleaseTimeout = LOCK_TIMEOUT;
    lockLocked = true;
}
void unlock(){
    servoUnlock();
    lockServoReleaseTimeout = LOCK_TIMEOUT;
    lockLocked = false;
}
LOCK_STATUS getLockStatus(){
    if(lockServoReleaseTimeout > 0) return MOVING;
    if(!lockLocked){
        if(getDoorIsClosed()){
            return UNLOCKED;
        }else{
            return OPENED_DOOR;
        }
    }else{
        if(getDoorIsClosed()){
            if(getThumbTurnIsLocked()){
                return LOCKED;
            }else{
                return INVALID_UNLOCKED;
            }
        }else{
            if(getThumbTurnIsLocked()){
                return INVALID_OPENED_DOOR;
            }else{
                return INVALID_UNLOCKED_OPENED_DOOR;
            }
        }
    }
}

void lockTick(){
    if(lockServoReleaseTimeout > 0) {
        lockServoReleaseTimeout--;
        Serial.printf("LT %d\n", lockServoReleaseTimeout);
    }
    if(lockServoReleaseTimeout == 0){
        servoRelease();
        lockServoReleaseTimeout = -1;
    }
}¥