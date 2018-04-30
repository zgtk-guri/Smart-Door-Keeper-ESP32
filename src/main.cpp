#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>
#include <WiFi.h>
#include <base64.h>
#include "WebServer.h"
#include "ST7032.h"
#include "PN532.h"
#include "PN532_SPI.h"
#include <ArduinoOTA.h>

#include "html_files.h"
#include "port_def.h"
#include "pref_key.h"
#include "switch.h"
#include "util.h"
#include "nfc.h"
#include "servo.h"
#include "ble.h"
#include "lock.h"

#include <vector>

const char* AP_SSID = "SmartDoorKeyper";
const char* WWW_USERNAME = "admin";

WebServer webServer;
ST7032 lcd;
Preferences pref;

enum States{
    ABORT = 0,
    LOCKED_STAND_BY,
    UNLOCKED_STAND_BY,
    SETUP,
    LOCk_POS_SETUP,
    ADD_CARD,
};

States state = ABORT;

TaskHandle_t switchPollingTaskHandle; 
TaskHandle_t nfcReadTaskHandle;
TaskHandle_t beepTaskHandle;

String settingPassword = "password";

std::vector<String> cardIds;
void reloadCards(){
    String idmsStr = pref.getString(PREF_CARD_IDMS, "");
    if(idmsStr.length() % 16 != 0) return;
    cardIds.clear();
    for(int i = 0; i < idmsStr.length() / 16; i++){
        cardIds.push_back(idmsStr.substring(i * 16, i * 16 + 16));
    }
}

void setup() {
    Serial.begin(115200);

    // port initialize
    initSwitches();

    digitalWrite(PORT_LED1, LOW);
    pinMode(PORT_LED1, OUTPUT);
    digitalWrite(PORT_LED2, LOW);
    pinMode(PORT_LED2, OUTPUT);
    digitalWrite(PORT_PN532_CS, HIGH);
    pinMode(PORT_PN532_CS, OUTPUT);
    digitalWrite(PORT_PN532_POW, LOW);
    pinMode(PORT_PN532_POW, OUTPUT);

    // lcd initialize
    Wire.begin(PORT_I2C_SDA, PORT_I2C_SCL);
    lcd.begin(8, 2);
    lcd.setContrast(30); 
    lcd.print("Smart D.");
    lcd.setCursor(0,1);
    lcd.print("  Keyper"); 
    
    // servo initialize
    servoInit();
    servoRelease();

    // switched initalize
    xTaskCreatePinnedToCore(switchPollingTask, "switchPolling", 1024, NULL, 1, &switchPollingTaskHandle, 1);

    // wifi initialize
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(IPAddress(192,168,4,2), IPAddress(0,0,0,0), IPAddress(255,255,255,0));
    WiFi.softAP(AP_SSID, settingPassword.c_str());

    // preferences initialize
    pref.begin(PREF_NAME, false);
    // Turn on DIP SW 1 to clear all preferences
    if(getDipSw1IsOn()){
        pref.clear();
        lcd.clear();
        lcd.print("Clear!!!");
        while(1) delay(1);
    }
    if(!pref.getBool(PREF_SET_UP, false)){
        state = SETUP;
    }else{
        // load settings
        settingPassword = pref.getString(PREF_SET_UP_PASSWORD, "passrord");
        WiFi.softAP(AP_SSID, settingPassword.c_str());
        if(pref.getBool(PREF_WIFI_SET_UP, false)){
            WiFi.begin(pref.getString(PREF_CONNECT_SSID, "").c_str(), pref.getString(PREF_CONNECT_PW, "").c_str());
        }
        setBlePin(pref.getString(PREF_BLE_PIN));
        state = LOCKED_STAND_BY;
    }
    if(!pref.getBool(PREF_LOCK_POS_SET_UP) && !getDipSw2IsOn()){
        lcd.clear();
        lcd.print("Lock Pos");
        lcd.setCursor(0,1);
        lcd.print("SW6 ON");
        state = ABORT;
        return;
    }
    if(getDipSw2IsOn()){
        state = LOCk_POS_SETUP;
        digitalWrite(PORT_SERVO_POW, HIGH);
        ledcWrite(SERVO_LEDC_CHANNEL, servoDegreeToDuty(90));
        return;
    }

    // web server initialize
    webServer.begin();
    webServer.on("/setting", [](){
        if(!webServer.authenticate(WWW_USERNAME, settingPassword.c_str())){
            return webServer.requestAuthentication();
        }
        webServer.send(200, "text/html", setting_html);
    });
    webServer.on("/", [](){
        if(!webServer.authenticate(WWW_USERNAME, settingPassword.c_str())){
            return webServer.requestAuthentication();
        }
        webServer.send(200, "text/html", index_html);
    });
    webServer.on("/open", [](){
        if(!webServer.authenticate(WWW_USERNAME, settingPassword.c_str())){
            return webServer.requestAuthentication();
        }
        unlock();
        webServer.send(200, "text/plain", "OK");
    });
    webServer.on("/close", [](){
        if(!webServer.authenticate(WWW_USERNAME, settingPassword.c_str())){
            return webServer.requestAuthentication();
        }
        lock();
        webServer.send(200, "text/plain", "OK");
    });
        webServer.on("/status", [](){
        if(!webServer.authenticate(WWW_USERNAME, settingPassword.c_str())){
            return webServer.requestAuthentication();
        }
        String msg;
        switch(getLockStatus()){
        case MOVING:
            msg = "施錠・解錠中";
            break;
        case LOCKED:
            msg = "施錠";
            break;
        case UNLOCKED:
            msg = "解錠";
            break;
        case OPENED_DOOR:
            msg = "解錠 ドア開";
            break;
        case INVALID_UNLOCKED:
            msg = "不正解錠";
            break;
        case INVALID_UNLOCKED_OPENED_DOOR:
            msg = "不正解錠 ドア開";
            break;
        }
        webServer.send(200, "text/plain", msg);
    });
    webServer.on("/setting/set", [](){
        if(!webServer.authenticate(WWW_USERNAME, settingPassword.c_str())){
            return webServer.requestAuthentication();
        }
        if(webServer.method() != HTTP_POST){
            webServer.send(405, "text/plain", "Request to /setting/set must be POST");
            return;
        }
        if(webServer.hasArg("wifissid") && webServer.hasArg("wifipasswd") && webServer.hasArg("settingpasswd")){
            String ssid = webServer.arg("wifissid");
            String wifipw = webServer.arg("wifipasswd");
            String settingpw = webServer.arg("settingpasswd");
            if(!ssid.equals("")){
                pref.putBool(PREF_WIFI_SET_UP, true);
                pref.putString(PREF_CONNECT_SSID, ssid);
                pref.putString(PREF_CONNECT_PW, wifipw);
            }
            pref.putBool(PREF_SET_UP, true);
            pref.putString(PREF_SET_UP_PASSWORD, settingpw);
            state = LOCKED_STAND_BY;
            webServer.send(200, "OK!");
        }else{
            webServer.send(400);
            return;
        }
    });
    webServer.on("/setting/key", [](){
        if(!webServer.authenticate(WWW_USERNAME, settingPassword.c_str())){
            return webServer.requestAuthentication();
        }
        String blePin = pref.getString(PREF_BLE_PIN);
        if(blePin.length() == 0){
            uint32_t raw[4];
            for(int i = 0; i < 4; i++){
                raw[i] = esp_random();
            }
            blePin = base64::encode((uint8_t *)raw, 15);
            pref.putString(PREF_BLE_PIN, blePin);
        }
        webServer.send(200, "text/plain", blePin);
    });

    // nfc initialize
    xTaskCreatePinnedToCore(nfcReadTask, "nfcRead", 1024, NULL, 2, &nfcReadTaskHandle, 1);
    reloadCards();

    // ble init
    initBle();

    if(state == LOCKED_STAND_BY){
        setServoPositions(pref.getInt(PREF_LOCK_POS), pref.getInt(PREF_RELEASE_POS));
        if(!getDoorIsClosed()){
        lcd.clear();
        lcd.print("Close");
        lcd.setCursor(0,1);
        lcd.print("the Door");
        while(!getDoorIsClosed()){
            webServer.handleClient();
        }
        lcd.clear();
            lcd.print("Smart D.");
            lcd.setCursor(0,1);
            lcd.print("  Keyper"); 
        }
        
        servoLock();
    }
    delay(1000);
    servoRelease();
}

void tickOnLockedStandBy(){
    static int page = 2;
    static int isMoving = false;
    LOCK_STATUS lockStatus = getLockStatus();
    if(millis() % 6000 < 2000){
        if(page == 2){
            lcd.clear();
            Serial.printf("State LOCKED, lockStatus = %d\n", lockStatus);
            if(lockStatus != LOCKED && lockStatus != UNLOCKED){
                lcd.print("Invalid");
            }
            lcd.setCursor(0,1);
            if(lockStatus == LOCKED){
                lcd.print("Locked");
            }else if(lockStatus == INVALID_OPENED_DOOR){
                lcd.print("DoorOpen");
            }else{
                lcd.print("Unlocked");
            }
            page = 0;
        }
    }else if(millis() % 6000 < 4000){
        if(page == 0){
            lcd.clear();
            lcd.print("Wi-Fi");
            lcd.setCursor(0,1);
            if(WiFi.isConnected()){
                lcd.print("RSSI:");
                lcd.print(WiFi.RSSI());
            }else{
                lcd.print("DisConn.");
            }
            page = 1;
        }
    }else if(page == 1){
        lcd.clear();
        lcd.print("BLE");
        lcd.setCursor(0,1);
        if(isBleAuthed()){
            lcd.print("Connect.");  
        }else{
            lcd.print("DisConn.");
        }
        page = 2;
    }

    if(lockStatus == LOCKED){
        digitalWrite(PORT_LED1, LOW);
    }else{
        digitalWrite(PORT_LED1, millis() % 500 < 250 ? HIGH : LOW);
    }

    if(isMoving && lockStatus != MOVING){
        state = UNLOCKED_STAND_BY;
        isMoving = false;
        return;
    }
    
    if(isSw1Clicked()){
        unlock();
        isMoving = true;
    }

    if(isSw2Clicked()){
        state = ADD_CARD;
    }
    if(isCardChanged() && getCardTouching()){
        String idm = idmToString(getCardId());
        for(int i = 0; i < cardIds.size(); i++){
            if(idm.equalsIgnoreCase(cardIds[i])){
                unlock();
                isMoving = true;
            }
        }
    }
}

void tickOnUnlockedStandBy(bool reset){
    static bool waitingCloseDoor;
    static int autoLockTimeout;
    static bool prevBleAuthed;
    static bool prevDoorClosed;
    static bool isMoving;
    
    bool authed = isBleAuthed();
    bool doorClosed = getDoorIsClosed();
    LOCK_STATUS lockStatus = getLockStatus();

    if(reset){
        Serial.println("State UNLOCKED, Reset");
        waitingCloseDoor = false;
        prevBleAuthed = authed;
        autoLockTimeout = authed ? -1 : 1000;
        prevDoorClosed = doorClosed;
        isMoving = false;
   }

    static int ledRefreshCount = 0;
    if(ledRefreshCount == 0){
        ledRefreshCount = 50;
        lcd.clear();
        if(authed){
            lcd.print("In Room.");
            lcd.setCursor(0,1);
            lcd.print("Unlocked");
        }else if(autoLockTimeout > 0){
            lcd.print("AutoLock");
            lcd.setCursor(0,1);
            lcd.print("in ");
            lcd.print(autoLockTimeout / 100);
            lcd.print("sec.");
        }else{
            lcd.setCursor(0,1);
            lcd.print("Unlocked");
        }
    }
    ledRefreshCount--;

    if(authed != prevBleAuthed || reset){
        if(authed){
            autoLockTimeout = -1;
        }else{
            if(doorClosed) autoLockTimeout = 1000;
        }
        prevBleAuthed = authed;
    }
    
    if(!authed){
        if(doorClosed != prevDoorClosed){
            if(doorClosed){
                autoLockTimeout = 500;
            }else{
                autoLockTimeout = -1;
            }
            prevDoorClosed = doorClosed;
        }
    }
    
    if(autoLockTimeout >= 0){
        autoLockTimeout--;
        if(autoLockTimeout == 0){
            waitingCloseDoor = true;
            autoLockTimeout = -1;
        }
    }
    
    if(isCardChanged() && getCardTouching()){
        String idm = idmToString(getCardId());
        for(int i = 0; i < cardIds.size(); i++){
            if(idm.equalsIgnoreCase(cardIds[i])){
                waitingCloseDoor = true;
            }
        }
    }
        
    if(isSw1Clicked() && getDoorIsClosed()){
        waitingCloseDoor = true;
    }

    if(waitingCloseDoor && doorClosed){
        for(int i = 0; i < 100; i++){
            delay(10);
            webServer.handleClient();
            if(!getDoorIsClosed()){
                return;
            }
        }
        lock();
        isMoving = true;
    }

    if(isMoving && lockStatus != MOVING) state = LOCKED_STAND_BY;
}

void tickOnSetup(){
    static int page = 2;
    if(millis() % 3000 < 1000){
        if(page == 2){
            lcd.clear();
            lcd.print("Connect ");
            lcd.setCursor(0,1);
            lcd.print("      to");
            page = 0;
        }
    }else if(millis() % 3000 < 2000){
        if(page == 0){
            lcd.clear();
            auto ip = WiFi.softAPIP();
            lcd.printf("%d.%d.", ip[0], ip[1]);
            lcd.setCursor(0,1);
            lcd.printf("%d.%d", ip[2], ip[3]);
            page = 1;
        }
    }else if(page == 1){
        lcd.clear();
        lcd.print("to");
        lcd.setCursor(0,1);
        lcd.print(" set up");
        page = 2;
    }
    isSw1Clicked();
    isSw2Clicked();
}

void tickOnLockPosSetup(){
    static int page = 1;
    static int position = 90;
    static int min = position;
    static int max = position;
    // LCD
    if(millis() % 2000 < 1000){
        if(page == 1){
            lcd.clear();
            lcd.print("Left:CCW");
            lcd.setCursor(0,1);
            lcd.print("Right:CW");
            page = 0;
        }
    }else{
        if(page == 0){
            lcd.clear();
            lcd.print("Lock Pos");
            lcd.setCursor(0,1);
            lcd.print("Set up");
            page = 1;
        }
    }
    
    // Servo
    if(getSw1IsPushed()){
        position--;
        if(position < 0) position = 0;
        if(position < min) min = position;
    }
    if(getSw2IsPushed()){
        position++;
        if(position > 180) position = 180;
        if(position > max) max = position;
    }
    ledcWrite(SERVO_LEDC_CHANNEL, servoDegreeToDuty(position));
    if(!getDipSw2IsOn()){
        lcd.clear();
        lcd.print("Done!!");
        ledcWrite(SERVO_LEDC_CHANNEL, servoDegreeToDuty(min));
        delay(500);
        if(getThumbTurnIsLocked()){
            pref.putInt(PREF_LOCK_POS, min);
            pref.putInt(PREF_RELEASE_POS, max);
        }else{
            pref.putInt(PREF_LOCK_POS, max);
            pref.putInt(PREF_RELEASE_POS, min);
        }
        pref.putBool(PREF_LOCK_POS_SET_UP, true);
        ESP.restart();
    }
    isSw1Clicked();
    isSw2Clicked();
}

void tickOnAddCard(){
    static int page = 2;
    if(millis() % 3000 < 1000){
        if(page == 2){
            lcd.clear();
            lcd.print("Add Card");
            lcd.setCursor(0,1);
            lcd.print("Mode");
            page = 0;
        }
    }else if(millis() % 3000 < 2000){
        if(page == 0){
            lcd.clear();
            lcd.print("Touch a ");
            lcd.setCursor(0,1);
            lcd.print("   card!");
            page = 1;
        }
    }else if(page == 1){
        lcd.clear();
        lcd.print("Push to ");
        lcd.setCursor(0,1);
        lcd.print("  cancel");
        page = 2;
    }

    if(isCardChanged() && getCardTouching()){
        String idm = idmToString(getCardId());
        Serial.println(idm);
        page = 3;
        lcd.clear();
        lcd.print(idm.substring(0,8));
        lcd.setCursor(0,1);
        lcd.print(idm.substring(8));
        delay(200);
        if(idm.length() == 16){
            String idms = pref.getString(PREF_CARD_IDMS, "");
            idms += idm;
            pref.putString(PREF_CARD_IDMS, idms);
        }
        reloadCards();  
        state = LOCKED_STAND_BY;
    }
}

States prevState = state;
void loop() {
    webServer.handleClient();
    States nowState = state;
    switch(nowState){
    case LOCKED_STAND_BY:
        tickOnLockedStandBy();
        break;
    case UNLOCKED_STAND_BY:
        tickOnUnlockedStandBy(nowState != prevState);
        break;
    case SETUP:
        tickOnSetup();
        break;
    case LOCk_POS_SETUP:
        tickOnLockPosSetup();
        break;
    case ADD_CARD:
        tickOnAddCard();
        break;
    }
    prevState = nowState;
    lockTick();
    delay(10);
}