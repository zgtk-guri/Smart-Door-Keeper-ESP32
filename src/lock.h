#ifndef LOCK_H
#define LOCK_H

enum LOCK_STATUS{
    MOVING,
    LOCKED,
    UNLOCKED,
    OPENED_DOOR,
    INVALID_OPENED_DOOR,
    INVALID_UNLOCKED,
    INVALID_UNLOCKED_OPENED_DOOR
};

const static int LOCK_TIMEOUT = 100;

void lock();
void unlock();
LOCK_STATUS getLockStatus();

void lockTick();

#endif