#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>
#include <WiFi.h>
#include "WebServer.h"
#include "ST7032.h"

#include "html_files.h"
#include "port_def.h"
#include "pref_key.h"
#include "switch.h"
#include "util.h"

const char* AP_SSID = "SmartDoorKeyper";
const char* AP_PASSWD = "password";

WebServer webServer;
ST7032 lcd;
Preferences pref;

enum States{
    UNKNOWN = 0,
    STAND_BY,
    SETUP,
};

States state = UNKNOWN;



TaskHandle_t switchPollingTaskHandle;

void setup() {
    // port initialize
    initSwitches();

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
    
    // servo initialize
    digitalWrite(PORT_SERVO_POW, HIGH);
    ledcSetup(SERVO_LEDC_CHANNEL, 50, 16);
    ledcAttachPin(PORT_SERVO_SIG, SERVO_LEDC_CHANNEL);
    ledcWrite(SERVO_LEDC_CHANNEL, servoDegreeToDuty(90));

    // switched initalize
    xTaskCreatePinnedToCore(switchPollingTask, "switchPolling", 1024, NULL, 1, &switchPollingTaskHandle, 1);

    // wifi initialize
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_SSID, AP_PASSWD);

    // preferences initialize
    pref.begin(PREF_NAME, false);
    if(!pref.getBool(PREF_SET_UP, false)){
        state = SETUP;
    }else{
        // loading settings
        
        state = STAND_BY;
    }

    // web server initialize
    webServer.begin();
    webServer.on("/skeleton.css", [](){
        webServer.send(200, "text/css", skelton_css);
    });
    webServer.on("/normalize.css", [](){
        webServer.send(200, "text/css", normalize_css);
    });
}

void loop() {
    webServer.handleClient();
}