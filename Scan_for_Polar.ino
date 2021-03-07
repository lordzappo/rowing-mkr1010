/*
  Scan

  This example scans for BLE peripherals and prints out their advertising details:
  address, local name, advertised service UUID's.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  Serial.println("BLE Central scan");

  // start scanning for peripheral
  // this is Polar H10 Address
  BLE.scanForAddress("fe:51:f4:e8:d3:0e");
}

void loop() {
  // check if a peripheral has been discovered
  
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
     BLE.stopScan();
    // discovered a peripheral
    Serial.println("Discovered a peripheral");
    Serial.println("-----------------------");

    // print address
    Serial.print("Address: ");
    Serial.println(peripheral.address());

    // print the local name, if present
    if (peripheral.hasLocalName()) {
      Serial.print("Local Name: ");
      Serial.println(peripheral.localName());
    }
    
    if (peripheral.connect()) {
      Serial.println("Connected");
      Serial.println("Discovering attributes ...");
      
    } else {
      Serial.println("Failed to connect!");
      return;
    }
    Serial.println("Discovering attributes ...");
    if (peripheral.discoverAttributes()) {
      Serial.println("Attributes discovered");
    } else {
      Serial.println("Attribute discovery failed!");
      peripheral.disconnect();
      return;
    }

    Serial.println("before service");
    BLEService hrService = peripheral.service("180d");
    Serial.println("After service");
    if (hrService) {
      BLECharacteristic hrRate = hrService.characteristic(0);
      Serial.print("\tCharacteristic ");
      Serial.print(hrRate.uuid());
      Serial.print(", properties 0x");
      Serial.print(hrRate.properties(), HEX);
      Serial.print(", Max Value Size: ");
      Serial.print(hrRate.valueSize());
      
      Serial.println();
      if (hrRate.canSubscribe()) {
        Serial.println("Can subscribe!");
        if (!hrRate.subscribe()) {
          peripheral.disconnect();
          return;
        }
        while(peripheral.connected()) {
          if (hrRate.valueUpdated()) {
            // Serial.println("Value Updated!");
            if (hrRate.valueLength() > 0) {
              // print out the value of the characteristic
              int returnSize = hrRate.valueLength();
              byte flags = hrRate.value()[0];
              if (flags >> 7) { //2 byte bpm
                Serial.print("double byte HR value: ");
                byte byte1 = hrRate.value()[1];
                byte byte2 = hrRate.value()[2];
                uint16_t hrRate = (byte1 <<8) + byte2;
                Serial.println(hrRate);
              }
              else {
                Serial.print("single byte HR value: ");
                Serial.println((uint8_t)hrRate.value()[1]);
              }              
            }
          }
        }
      }
    }
    else {
      Serial.println("Failed to get HRService 180d");
      return;
    }
    
   }
}


void printData(const unsigned char data[], int length) {
  for (int i = 0; i < length; i++) {
    unsigned char b = data[i];

    if (b < 16) {
      Serial.print("0");
    }

    Serial.print(b);
  }
}
