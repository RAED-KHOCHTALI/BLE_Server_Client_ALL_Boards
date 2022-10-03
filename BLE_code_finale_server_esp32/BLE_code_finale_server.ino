
//we can only connect to one device 
//we can't perfom communication between 2 serial esp32
#include "BluetoothSerial.h" //Header File for Serial Bluetooth, will be added by default into Arduino
#include <DHT.h>

#define DHTPIN 25
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
char tchar[4];
uint8_t arr;
uint8_t c;

BluetoothSerial ESP_BT; //Object for Bluetooth
bool isconnected = false ;
int data;
bool nouveau = false;
int LED_BUILTIN = 2;
char received[100];
byte  j = 0;
byte k = 0;


void setup() {
  Serial.begin(9600); //Start Serial monitor in 9600
  ESP_BT.begin("ESP32_Test_Serial"); //Name of your Bluetooth Signal
  Serial.println("Bluetooth Device is Ready to Pair");
  pinMode (LED_BUILTIN, OUTPUT);//Specify that LED pin is output

  dht.begin();


}

void loop() {
  if (ESP_BT.connected() && (isconnected == false))
  {
    Serial.println("connected to smartphone");
    Serial.println("ready to read data from smartphone");
    isconnected = true;
    analogWrite(LED_BUILTIN, 255);
  }
  if (!(ESP_BT.connected()) && isconnected)
  { isconnected = false;
    Serial.println("disconnected from smartphone");
    for (int i = 0; i < 256; i++) {
      delay(5);
      analogWrite(LED_BUILTIN, 255 - i);
    }
  }
  if (isconnected) {
    float t = dht.readTemperature();  //read the value of the potentiometer
    
    dtostrf(t, 1, 2, tchar); // float_val, min_width, digits_after_decimal, char_buffer
    arr = ((tchar[0] - '0') * 10) + (tchar[1] - '0');
    if(c==0){
      c++;
    Serial.print("we are sending temperature=");
    Serial.println(t);}
    ESP_BT.write(arr);
    if (ESP_BT.available()) //Check if we receive anything from Bluetooth
    {
      data = ESP_BT.read(); //Read what we recevive
      received[j] = data;
      j++;
      if (data == 10) {
        nouveau = true;
      }
    }

    if (nouveau) {
      Serial.println("Received:");
      Serial.print(received);
      nouveau = false;
        j = 0;
    }
  }
}
