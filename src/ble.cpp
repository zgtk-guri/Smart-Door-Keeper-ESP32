#include "ble.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

bool bleAuthed = false;
String blePin = "";

class MySecurity : public BLESecurityCallbacks {

	uint32_t onPassKeyRequest(){
        ESP_LOGI(LOG_TAG, "PassKeyRequest");
		return 123456;
	}
	void onPassKeyNotify(uint32_t pass_key){
        ESP_LOGI(LOG_TAG, "The passkey Notify number:%d", pass_key);
	}
	bool onConfirmPIN(uint32_t pass_key){
        ESP_LOGI(LOG_TAG, "The passkey YES/NO number:%d", pass_key);
	    vTaskDelay(5000);
		return true;
	}
	bool onSecurityRequest(){
		return true;
	}

	void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl){
		
	}
};

class BleConnectCallbacks : public BLEServerCallbacks{
	void onConnect(BLEServer* pServer){

	}

	void onDisconnect(BLEServer* pServer){
		bleAuthed = false;
	}
};

class PinCodeCharacteristicCallbacks : public BLECharacteristicCallbacks{
	void onWrite(BLECharacteristic* pCharacteristic){
		std::string value = pCharacteristic->getValue();
		if(blePin.equals(value.c_str())){
			bleAuthed = true;
		}
	}
};

#define SDK_SERVICE_UUID "71495a29-c548-4aa6-a75b-61959767faa2"
#define SDK_PIN_UUID "1775cd75-a329-4607-a652-9a0a69ba77c8"

void initBle(){
    BLEDevice::init("SmartDoorKeyper");
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_MITM);
    BLEDevice::setSecurityCallbacks(new MySecurity());
    BLEServer *bleServer = BLEDevice::createServer();
	bleServer->setCallbacks(new BleConnectCallbacks());
    BLEService *sdkService = bleServer->createService(SDK_SERVICE_UUID);
    BLECharacteristic *pinCode = sdkService->createCharacteristic(SDK_PIN_UUID, BLECharacteristic::PROPERTY_WRITE);
	pinCode->setCallbacks(new PinCodeCharacteristicCallbacks());
	sdkService->start();
    BLEAdvertising *advertising = bleServer->getAdvertising();
	advertising->start();

    BLESecurity *bleSecurity = new BLESecurity();
    bleSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
	bleSecurity->setCapability(ESP_IO_CAP_IO);
	bleSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
}

bool isBleAuthed(){
	return bleAuthed;
}

void setBlePin(String pin){
	blePin = pin;
}