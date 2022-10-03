/*

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.
*/

#include <BLE2902.h> // to ba able to use the notify option
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h> // functions needed to configure a client
#include <BLEServer.h> // functions needed to configure a server
#include <BLEUtils.h> //A set of general %BLE utilities.

BLEServer* pServer = NULL;

int i = 0; // counter for devices connected

#define LED 2

uint8_t level = 57;

BLECharacteristic* pCharacteristic_temp;

#define BatteryService BLEUUID((uint16_t)0x180F)
#define BatteryLevelCharacteristic BLEUUID((uint16_t)0x2A19)

//#define my_telephone "53:E3:0E:01:B3:68"
bool my_telephone_b = false;
bool my_esp_b = false;
bool my_bgm220p_b = false ;
bool my_raspi_b=false;



#define my_esp32  "10:52:1C:74:4C:B6"
#define my_bgm220p "60:A4:23:C9:82:95"
#define my_raspi "70:C9:4E:4C:0A:20"

bool deviceConnected = false;
// how to know number of devices

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t* param)
    {
     String t;

        BLEDevice::startAdvertising();
        Serial.print("the device of adress mac =     ");
        deviceConnected = true;
        i++;
        for (int j = 0; j < 6; j++) {

            char str[3];

            sprintf(str, "%02X", (int)param->connect.remote_bda[j]);
            Serial.print(str);
            t = t + str + ':';

            if (j < 5) {
                Serial.print(":");
            }
        }

        Serial.println("     is connected");
        Serial.print("We are sending data to ");
        Serial.print(i);
        if (i == 1)
            Serial.println(" device.");
        else
            Serial.println(" devices.");

         t[t.length()-1] = '\0';

        analogWrite(LED, 70 * i);
    };

    void onDisconnect(BLEServer* pServer)
    {
        BLEDevice::startAdvertising(); // restart advertising
        deviceConnected = false;
        i--;
        analogWrite(LED, 70 * i);
        Serial.print("Device disconnected. Now there are ");
        Serial.print(i);
        Serial.println(" devices connected.");
        if (i == 0) {
            Serial.println("no devices connected");
            for (int j = 0; j < 256; j++) {
                delay(5);
                analogWrite(LED, 255 - j);
            }
        }
    }
};

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic_data)
    {
        std::string rxValue = pCharacteristic_data->getValue();

        if (rxValue.length() > 0) {
            Serial.println("*********");
            Serial.print("Received Value: ");

            for (int i = 0; i < rxValue.length(); i++) {
                Serial.print(rxValue[i]);
            }

            Serial.println();
        }
    }
};

void setup()
{
    pinMode(LED, OUTPUT);
    Serial.begin(9600);

    // Create the BLE Device
    BLEDevice::init("ESP32 Test"); // Give it a name

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService* pService = pServer->createService(BatteryService);

    // Create a BLE Characteristic
   pCharacteristic_temp = pService->createCharacteristic(BatteryLevelCharacteristic, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ );
 
    BLE2902* m1_BLE2902 = new BLE2902();
    pCharacteristic_temp->addDescriptor(m1_BLE2902);
   
    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BatteryService);
    pServer->startAdvertising();
    Serial.println("BLE is now advertising");
    Serial.println("Waiting a client connection to connect...");
}

void loop()
{
    
    if (deviceConnected) {
      
          pCharacteristic_temp->setValue(&level, 1);
          level++;
          delay(50);
          pCharacteristic_temp->notify(true);
          
           if (int(level)==100)
             level=0;  
    }

}
