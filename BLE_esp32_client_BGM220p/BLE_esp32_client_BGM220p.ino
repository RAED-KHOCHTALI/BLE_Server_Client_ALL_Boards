#include "BLEDevice.h"

// BLE Server name (the other ESP32 name running the server sketch)
#define bleServerName "BGM220P_TEST"
#define bleAdress "60:a4:23:c9:82:95"
String Scaned_BLE_Address;

/* UUID's of the service, characteristic that we want to read*/
// BLE Service
static BLEUUID ServiceUUID("cd9bbfe1-2f68-458a-98d3-617938ed0b28");
static BLEUUID temperatureCharacteristicUUID("0ef54135-c6ee-4fed-9f26-f52d49c27117");
//static BLEUUID humidityCharacteristicUUID("89beec2d-0612-4ec2-a1c7-9f19539aff86");
static BLEUUID dataCharacteristicUUID("db3f9381-5155-4d91-8648-cd00e791acc3");
static BLEAdvertisedDevice* myDevice;
static BLEAddress *Server_BLE_Address;
static BLEClient* pClient = BLEDevice::createClient();

int x;
// Flags stating if should begin connecting and if the connection is up
static boolean doConnect = false;
static boolean connected = false;

// Address of the peripheral device. Address will be found during scanning...
static BLEAddress* pServerAddress;
static BLEScan* pBLEScan;
// Characteristicd that we want to read
static BLERemoteCharacteristic* temperatureCharacteristic;
//static BLERemoteCharacteristic* humidityCharacteristic;
static BLERemoteCharacteristic* dataCharacteristic;

// Variables to store temperature and humidity
char* temperatureChar;
//char* humidityChar;

//std::string humi;
uint8_t temp;

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pClient)
    {

      connected = true;
      Serial.println("Connected");
    }

    void onDisconnect(BLEClient* pClient)
    {
      Serial.println("Disconnected");
    }
};

// Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      Serial.printf("Advertised Device: %s", advertisedDevice.toString().c_str());
      Server_BLE_Address = new BLEAddress(advertisedDevice.getAddress());
      Scaned_BLE_Address = Server_BLE_Address->toString().c_str();
      
      if ((Scaned_BLE_Address == bleAdress) || (advertisedDevice.getName()==bleServerName)) { // Check if the name of the advertiser matches
        advertisedDevice.getScan()->stop(); // Scan can be stopped, we found what we are looking for
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        pServerAddress = new BLEAddress(advertisedDevice.getAddress()); // Address of advertiser is the one we need
        doConnect = true; // Set indicator, stating that we are ready to connect
        Serial.println("Device found. Connecting!");
        int rssii;
        //int8_t  tx;
        double distance;
        if (advertisedDevice.haveRSSI()) {
          rssii = advertisedDevice.getRSSI();
          Serial.printf("Rssi: %d \n", (int)advertisedDevice.getRSSI());
        }
     distance =pow(10,(double)(-72-(rssii))/(double)40);
       Serial.printf("distance=%2.2fm\n",distance);
      }
      else
        Serial.println("we didn't find the specific device");
    }
};


// Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEClient* pclient)
{
  Serial.print("Connecting to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  pclient->setClientCallbacks(new MyClientCallback());
  Serial.println("-Created CLIENT ");
  // Connect to the remove BLE Server.
  bool ok = pclient->connect(myDevice);
  if (!ok) {
    Serial.print("Failed to connect to server: ");
    Serial.println(myDevice->getAddress().toString().c_str());
    pclient->disconnect();
    return false;
  }
  Serial.println("Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pclient->getService(ServiceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(ServiceUUID.toString().c_str());
    pclient->disconnect();
    return (false);
  }

  // Obtain a reference to the characteristics in the service of the remote BLE server.
  temperatureCharacteristic = pRemoteService->getCharacteristic(temperatureCharacteristicUUID);
  //humidityCharacteristic = pRemoteService->getCharacteristic(humidityCharacteristicUUID);
  dataCharacteristic = pRemoteService->getCharacteristic(dataCharacteristicUUID);
  if (dataCharacteristic == nullptr )//|| humidityCharacteristic == nullptr) {
    {Serial.print("Failed to find our characteristic UUID");
    pclient->disconnect();
    return false;
  }
  Serial.println("RECEIVED temperature  and ready to send data");
  connected = true;
  return true;
}

void setup()
{

  Serial.begin(9600);
  Serial.println("Start searching for devices");

  // Init BLE device
  BLEDevice::init("ESP32 receive");
  scanForServer();
}
void scanForServer()
{

  Serial.println("Scanning for Server...");
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop()
{
  if (doConnect == true) {
    if (connectToServer(pClient)) {
      Serial.println("Our esp32 is working as Client and everything is fine ");
      Serial.println("type your command ");
    } else {
      Serial.println(
        "We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }
  if (connected) {
  if (Serial.available() > 0) {
    char c = Serial.read();
      switch (c) {
        case 'd' :
          pClient->disconnect();
          Serial.println("disconnected to server");
          break;
        case 't' :
          temp = dataCharacteristic->readUInt8();
          Serial.print("temperature=");
          Serial.println(temp);
          break;
        // case 'h' :
        //   humi = humidityCharacteristic->readValue();
        //   Serial.print("humidity=");
        //   Serial.println(humi.c_str());
        //   break;
        case 'c' :
          pClient->disconnect();
          connected=false;
          break;
        case 's' :
          sendData("raed");
          break;
      }
    }}
    else {
      pClient->disconnect();
      scanForServer();
    }
}
void sendData(String stringToSend)
{
  // Only send when connected
  if (connected) {
    // Set the characteristic's value to be the array of bytes that is actually a string.
    temperatureCharacteristic->writeValue(stringToSend.c_str(), stringToSend.length());
    Serial.print("<<< ");
    Serial.println(stringToSend);

  } else {
    Serial.println("Can't Send because not connected");
    Serial.print("XXX ");
    Serial.println(stringToSend);
  }
}
