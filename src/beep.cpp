#include "beep.h"
#include <arduino.h>
#include "port_def.h"

QueueHandle_t beepQueue;

void beep1Short(int freq){
    int zero = 0;
    xQueueSend(beepQueue, &freq, portMAX_DELAY);
    xQueueSend(beepQueue, &zero, portMAX_DELAY);
}

void beep1Long(int freq){
    int zero = 0;
    for(int i = 0; i < BEEP_LONG_LEN; i++) xQueueSend(beepQueue, &freq, portMAX_DELAY);
    xQueueSend(beepQueue, &zero, portMAX_DELAY);
}
void beep2ShortShort(int freq1, int freq2){
    int zero = 0;
    xQueueSend(beepQueue, &freq1, portMAX_DELAY);
    xQueueSend(beepQueue, &zero, portMAX_DELAY);
    xQueueSend(beepQueue, &freq2, portMAX_DELAY);
    xQueueSend(beepQueue, &zero, portMAX_DELAY);
}
void beep2ShortLong(int freq1, int freq2){
    int zero = 0;
    xQueueSend(beepQueue, &freq1, portMAX_DELAY);
    xQueueSend(beepQueue, &zero, portMAX_DELAY);
    for(int i = 0; i < BEEP_LONG_LEN; i++) xQueueSend(beepQueue, &freq2, portMAX_DELAY);
    xQueueSend(beepQueue, &zero, portMAX_DELAY);
}

void initBeep(){
    digitalWrite(PORT_BUZZER, LOW);
    pinMode(PORT_BUZZER, OUTPUT);

    ledcSetup(BUZZER_LEDC_CHANNEL, 50, 16);
    ledcAttachPin(PORT_BUZZER, BUZZER_LEDC_CHANNEL);
}

void beeptask(void *pvParameters){
    beepQueue = xQueueCreate(32, sizeof(int));

    while(1){
        int freq;
        xQueueReceive(beepQueue, &freq, portMAX_DELAY);
        Serial.printf("beep: freq=%d\n", freq); 
        ledcWriteTone(BUZZER_LEDC_CHANNEL, freq);
        delay(100);
    }
}