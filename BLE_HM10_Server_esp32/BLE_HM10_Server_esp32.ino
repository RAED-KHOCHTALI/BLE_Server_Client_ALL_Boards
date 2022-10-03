#include <SoftwareSerial.h>
SoftwareSerial ble_device(3, 2);
#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

char tchar[4];
uint8_t arr;
uint8_t c;

int data;
bool nouveau = false;

char received[100];
byte  j = 0;

String str_ii = "";
int ii_0 = 0;

void setup() {
  dht.begin();
  Serial.begin(9600);
  ble_device.begin(9600);
  Serial.println("we are sending data via bluetooth");
  delay(100);

  // Enter AT+ commands of interest here (BLE Address, UUIDs, Power settings)
  ble_cmd("AT+NAMEBLExAR", "Device Name: "); // printout device name
  ble_cmd("AT+LADDR", "BLE address: "); // printout BLE address
  ble_cmd("AT+UUID", "SERVER UUID: "); // printout SERVER UUID
  ble_cmd("AT+CHAR", "Char UUID: "); // printout character UUID
  ble_cmd("AT+VERSION", "Version: "); // module version
  delay(500);
}

void loop() {

  if (ble_device.available()) //Check if we receive anything from Bluetooth
  {
    data = ble_device.read(); //Read what we recevive
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
  float t = dht.readTemperature();  //read the value of the potentiometer

  dtostrf(t, 1, 2, tchar); // float_val, min_width, digits_after_decimal, char_buffer
  arr = ((tchar[0] - '0') * 10) + (tchar[1] - '0');
  if (c == 0) {
    c++;
    Serial.print("we are sending temperature=");
    Serial.println(t);
  }
  ble_device.write(arr);

}



String ble_cmd(String cmd_str, String desc_str) {
  str_ii = "";
  unsigned long t1 = millis();
  ble_device.println(cmd_str);
  while (true) {
    char in_char = ble_device.read();
    if (int(in_char) == -1 or int(in_char) == 42) {
      if ((millis() - t1) > 2000) { // 2 second timeout
        return "Err";
      }
      continue;
    }
    if (in_char == '\n') {
      Serial.print("Bluetooth " + desc_str);
      Serial.println(str_ii.substring(0, str_ii.length()));
      return str_ii;
    }
    str_ii += in_char;
  }
}
