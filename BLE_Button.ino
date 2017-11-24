/*
 * Button Bluetooth Low-Energy Peripheral
 * 
 * This Genuino will act as a peripheral. It will connect with the central and write a
 * message to toggle the central's LED state.
 * 
 * Connect ground through a 1K resistor to one side of the button and to pin 8.
 * Connect the other side of the button to positive.
 * If in doubt, look up how to connect a button to an arduino, and do this for pin 8.
 *
 */

#include <CurieBLE.h>

const int buttonPin = 8;
bool addressShown = true;
char buttonValue = 0;
char newButtonValue = 0;

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);

  // initialize the BLE hardware
  BLE.begin();
  Serial.println("BLE.begin()");

  // start scanning for peripherals
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  Serial.println("BLE.scanForUuid();");
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    // stop scanning
    BLE.stopScan();

    controlPeripheral(peripheral);

    // peripheral disconnected, start scanning again
    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  }
}

void controlPeripheral(BLEDevice peripheral) {

  // 
  // Connect to the peripheral
  // 
  
  Serial.print("Connecting...  ");
  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  //
  // Discover peripheral attributes
  //
  
  Serial.print("Discovering attributes...   ");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered.");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  //
  // Retrieve the button characteristic
  // 

  Serial.print("Searching for led characteristic...  ");
  BLECharacteristic ledCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");

  if (!ledCharacteristic) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (!ledCharacteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (ledCharacteristic) {
    Serial.println("Found.");
  }

// 
// Connection stages completed.
// Ready to start updating the buttonState value.
//

  Serial.println("Peripheral is now fully connected.");
  
  while (peripheral.connected()) {

    if (peripheral&&!addressShown) {
      addressShown=true;
      Serial.print("Waiting for ");
      Serial.println(peripheral.address());
    }    
    
    peripheral.poll();  
    newButtonValue = digitalRead(buttonPin);
      
    boolean buttonChanged = (newButtonValue != buttonValue);
  
    if (buttonChanged) {
      buttonValue = newButtonValue;
      Serial.print("LED Characteristic = ");
      Serial.println( buttonValue, DEC );

      // if button is pressed, change the central's characteristic!
      if( buttonValue == 1 ){
        ledCharacteristic.setValue("1");      
      } else {
        ledCharacteristic.setValue("0");  
      }

    }
  }

  Serial.println("Peripheral disconnected");
}
