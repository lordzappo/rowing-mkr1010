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
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Clcd.h> // i2c LCD i/o class header
hd44780_I2Clcd lcd;
const int LCD_COLS = 40;
const int LCD_ROWS = 2;

struct hrmonitor
{
   BLEDevice* peripheral;
   BLECharacteristic* hrRate;
   bool isInitialized;
   
};

typedef struct hrmonitor HrMonitor;

void setupLCD() {
  int status;
  status = lcd.begin(LCD_COLS, LCD_ROWS);
	if(status) // non zero status means it was unsuccesful
	{
		// begin() failed so blink error code using the onboard LED if possible
		hd44780::fatalError(status); // does not return
	}

	// Print a message to the LCD
    lcd.setBacklight(0);
    lcd.setContrast(255);
  
}

uint8_t parseHrData(BLECharacteristic* hrRate) {
  // Serial.println("Value Updated!");
  if (hrRate->valueLength() > 0) {
    // print out the value of the characteristic
    int returnSize = hrRate->valueLength();
    byte flags = hrRate->value()[0];
    if (flags >> 7) { //2 byte bpm
      Serial.print("double byte HR value: ");
      byte byte1 = hrRate->value()[1];
      byte byte2 = hrRate->value()[2];
      uint16_t hrRate = (byte1 <<8) + byte2;
      return (uint8_t)hrRate;
    }
    else {
      Serial.print("single byte HR value: ");
      return (uint8_t)hrRate->value()[1];
    }              
  }
}

 // check if a peripheral has been discovered
HrMonitor connectToHrMonitor(HrMonitor &hrMonitor) {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
     BLE.stopScan();
    // discovered a peripheral
    // Serial.println("Discovered a peripheral");
    // Serial.println("-----------------------");

    // // print address
    // Serial.print("Address: ");
    // Serial.println(peripheral.address());

    // print the local name, if present
    // if (peripheral.hasLocalName()) {
    //   Serial.print("Local Name: ");
    //   Serial.println(peripheral.localName());
    // }
    
    if (peripheral.connect()) {
      Serial.println("Connected");
      Serial.println("Discovering attributes ...");
      
    } else {
      Serial.println("Failed to connect!");
    }
    Serial.println("Discovering attributes ...");
    if (peripheral.discoverAttributes()) {
      Serial.println("Attributes discovered");
    } else {
      Serial.println("Attribute discovery failed!");
      peripheral.disconnect();
    }

    BLEService hrService = peripheral.service("180d");
    if (hrService) {
      BLECharacteristic hrRate = hrService.characteristic(0);
      // Serial.print("\tCharacteristic ");
      // Serial.print(hrRate.uuid());
      // Serial.print(", properties 0x");
      // Serial.print(hrRate.properties(), HEX);
      // Serial.print(", Max Value Size: ");
      // Serial.print(hrRate.valueSize());
      
      Serial.println();
      if (hrRate.canSubscribe()) {
        Serial.println("Can subscribe!");
        if (!hrRate.subscribe()) {
          Serial.println("Subscribe failed");
        }
        else {
          hrMonitor.peripheral=&peripheral;
          hrMonitor.hrRate=&hrRate;
          hrMonitor.isInitialized=true;
        }
      }
    }
  }
  return hrMonitor;
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

void setup() {
   setupLCD();
  Serial.begin(11200);
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

// Men:
// Calories per minute = (-55.0969 + (0.6309 * Heart rate) + (0.1988 * Weight) + (0.2017 * Age)) / 4.184

// Women:
// Calories per Minute = (-20.4022 + (0.4472 * Heart Rate) - (0.1263 * Weight) + (0.074 * Age)) / 4.184

//Rowing distance
//C = 9.6367 * (D/T)^3 + 300 

uint8_t hr_value;
HrMonitor hrMonitor = {NULL, NULL, false};
void loop() {
  if (!hrMonitor.isInitialized) { 
    hrMonitor = connectToHrMonitor(hrMonitor);
  }
  if (hrMonitor.isInitialized) {
    if (hrMonitor.hrRate->valueUpdated()) {
    hr_value = parseHrData(hrMonitor.hrRate);
    }
  }
}
