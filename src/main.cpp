#include <Arduino.h>
#include <Wire.h>
#include "ST7032.h"
#include "port_def.h"

ST7032 lcd;

void setup() {
    // port initialize
    pinMode(PORT_SW1, INPUT_PULLUP);
    pinMode(PORT_SW2, INPUT_PULLUP);
    pinMode(PORT_DIPSW1, INPUT_PULLUP);
    pinMode(PORT_DIPSW2, INPUT_PULLUP);
    pinMode(PORT_DOOR_SW, INPUT_PULLUP);
    pinMode(PORT_LOCK_DET, INPUT_PULLUP);

    digitalWrite(PORT_LED1, LOW);
    pinMode(PORT_LED1, OUTPUT);
    digitalWrite(PORT_LED2, LOW);
    pinMode(PORT_LED2, OUTPUT);
    digitalWrite(PORT_BUZZER, LOW);
    pinMode(PORT_BUZZER, OUTPUT);
    digitalWrite(PORT_SERVO_POW, LOW);
    pinMode(PORT_SERVO_POW, OUTPUT);
    digitalWrite(PORT_SERVO_SIG, LOW);
    pinMode(PORT_SERVO_SIG, OUTPUT);
    digitalWrite(PORT_PN532_CS, HIGH);
    pinMode(PORT_PN532_CS, OUTPUT);

    // lcd initialize
    Wire.begin(PORT_I2C_SDA, PORT_I2C_SCL);
    lcd.begin(8, 2);
    
}

void loop() {

}