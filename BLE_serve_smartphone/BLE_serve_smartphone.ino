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
#include <BLEClient.h>
#include <BLEDevice.h> // functions needed to configure a client
#include <BLEScan.h>
#include <BLEServer.h> // functions needed to configure a server
#include <BLEUtils.h> //A set of general %BLE utilities.

BLEServer* pServer = NULL;

int i = 0; // counter for devices connected
int c = 0; // to print the temperature and humidity one time
#include <DHT.h>
#define DHTPIN 25
#define DHTTYPE DHT11
#define LED 2
DHT dht(DHTPIN, DHTTYPE);
char tchar[8];
char hchar[8];

int scanTime = 30;

BLECharacteristic* pCharacteristic_temp;
BLECharacteristic* pCharacteristic_humi;
BLECharacteristic* pCharacteristic_data;

#define SERVICE_UUID "92053ecb-cd1b-4819-aed4-e3e516d14789"
#define CHARACTERISTIC_UUID_data "e39d897c-ebca-473a-87ed-03a6bf9f62c6"
#define CHARACTERISTIC_UUID_temp "6781f563-0dc3-4a2a-bd58-ad5ce98c6b22"
#define CHARACTERISTIC_UUID_humi "89beec2d-0612-4ec2-a1c7-9f19539aff86"
//#define my_telephone "53:E3:0E:01:B3:68"
bool my_telephone_b = false;
bool my_esp_b = false;


#define my_esp32  "10:52:1C:74:4C:B6"


bool deviceConnected = false;
// how to know number of devices
class MyServerCallbacks : public BLEServerCallbacks {
     void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t* param)
    {

        BLEDevice::startAdvertising();
        String t;
        Serial.print("the device of adress mac =     ");
        deviceConnected = true;
        i++;
        for (int j = 0; j < 6; j++) {

            char str[3];

            sprintf(str, "%02X", (int)param->connect.remote_bda[j]);
            t = t + str + ':';
            Serial.print(str);

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
        
       /* if(t == my_telephone){
            my_telephone_b = true;
          Serial.println("OH THIS IS ACTUALLY MY PHONE ");
          }*/
        if(t == my_esp32){
            my_esp_b = true;
          Serial.println("OH THIS IS ACTUALLY MY ESP32 ");
          }
          else
          {
            my_telephone_b = true;
            Serial.println("OH THIS IS ACTUALLY MY PHONE ");
            }
          if(my_telephone_b & my_esp_b)
                    Serial.println("my PHONE and my ESP32 are CONNECTED");
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
  dht.begin();
  Serial.begin(9600);

  // Create the BLE Device
  BLEDevice::init("ESP32 Test"); // Give it a name

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic_temp = pService->createCharacteristic(CHARACTERISTIC_UUID_temp, BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_temp->addDescriptor(new BLE2902());

  pCharacteristic_humi = pService->createCharacteristic(CHARACTERISTIC_UUID_humi, BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_humi->addDescriptor(new BLE2902());

  pCharacteristic_data = pService->createCharacteristic(
                           CHARACTERISTIC_UUID_data, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY );
  pCharacteristic_data->addDescriptor(new BLE2902());
  // list of types of characteristic
  pCharacteristic_data->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->startAdvertising();
  Serial.println("BLE is now advertising");
  Serial.println("Waiting a client connection to connect...");
}

void loop()
{
  float t = dht.readTemperature(); // read the value of the potentiometer
  float h = dht.readHumidity();
  //  Serial.println(t);
  //  Serial.println(h);
  if (deviceConnected) {
    //        float h = getRssi();
     //     Serial.println(h);
    dtostrf(t, 1, 2, tchar); // float_val, min_width, digits_after_decimal, char_buffer
    dtostrf(h, 1, 2, hchar);

    pCharacteristic_temp->setValue(tchar);
    pCharacteristic_humi->setValue(hchar);

    pCharacteristic_temp->notify(); // Send the value to the app!
    pCharacteristic_humi->notify();

  }
 // else Serial.println("errorrr");
}
