#include "BLEDevice.h"
int x;
std::string temp;
#define HM_MAC "88:25:83:F1:0E:90"
#define led 2
// Service und Characteristic des HM-10 Moduls
static BLEUUID serviceUUID("0000FFE0-0000-1000-8000-00805F9B34FB");
static BLEUUID charUUID("0000FFE1-0000-1000-8000-00805F9B34FB");

static boolean connect = true;
static boolean connected = false;

static BLEAddress *pServerAddress;
static BLERemoteCharacteristic* pRemoteCharacteristic;
BLEClient*  pClient;

//    BLE Callbacks
class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pClient)
    {

      connected = true;
      digitalWrite(led, HIGH);
      Serial.println("Connected");
    }

    void onDisconnect(BLEClient* pClient)
    {
      connected=true;
      digitalWrite(led, LOW);
      Serial.println("Disconnected");
    }
};

static void notifyCallback
(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify)
{
  String data = "";
  for (int i = 0; i < length; i++)data += char(*pData++); // Append byte as character to string. Change to the next memory location
  Serial.println(data);
}

//Connect to BLE Server

bool connectToServer(BLEAddress pAddress)
{
  Serial.println("Trying to Connect with.... ");
  Serial.println(pAddress.toString().c_str());
  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
  pClient->connect(pAddress);
 

  // Obtaining a reference to required service
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);

  if (pRemoteService == nullptr)
  {
    Serial.print("we haven't found the desired device ");
    Serial.println(serviceUUID.toString().c_str());
    return false;
  }

  // reference to required characteristic
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("we are not capable of receiveing characterics");
    Serial.println(charUUID.toString().c_str());
    return false;
  }

 // pRemoteCharacteristic->registerForNotify(notifyCallback);
  return true;
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Start searching for devices");
  BLEDevice::init("ESP32 receive from HM10");
  pinMode(led, OUTPUT);

}

void loop()
{
  if (connect == true)
  {
    pServerAddress = new BLEAddress(HM_MAC);
    if (connectToServer(*pServerAddress))
    {
      connected = true;
      connect = false;
    }
    else
    {
      Serial.println("Connection does not work");
    }
  }

  if (connected) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      switch (c) {
        case 'd' :
          pClient->disconnect();
          break;
        case 't' :
          temp = pRemoteCharacteristic->readValue();
          Serial.print("temperature=");
          Serial.println(temp.c_str());
          break;
        case 'c' :
          connect = true;
          break;
        case 's' :
          sendData("raed");
          break;
      }
    }

  }
}
void sendData(String stringToSend)
{
  // Only send when connected
  if (connected) {
    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(stringToSend.c_str(), stringToSend.length());
    Serial.print("<<< ");
    Serial.println(stringToSend);

  } else {
    Serial.println("Can't Send because not connected");
    Serial.print("XXX ");
    Serial.println(stringToSend);
  }
}
