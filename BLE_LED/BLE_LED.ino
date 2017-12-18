/*
 * 
 * LED Bluetooth Low-Energy Central
 * 
 * This Genuino will act as a central. It will look for its peripheral, the button, and
 * allow this peripheral to change its state.
 * 
 * There are no connections necessary for this Genuino.
 * 
 */

#include <CurieBLE.h>

BLEPeripheral blePeripheral; // create peripheral instance
BLEService ledService( "19b10000-e8f2-537e-4f6c-d104768a1214 "); // create service
BLECharCharacteristic ledCharacteristic(" 19b10001-e8f2-537e-4f6c-d104768a1214 ", BLERead | BLEWrite );

bool addressShown = false;
const int ledPin = 13;
bool ledValue = false;

void setup() {
  
  Serial.begin( 9600 );
  Serial.println( "Serial communication started." );
  pinMode( ledPin, OUTPUT );
  delay( 1000 );

  // set the local name peripheral advertises
  blePeripheral.setLocalName( "LED Service" );
  // set the UUID for the service this peripheral advertises:
  blePeripheral.setAdvertisedServiceUuid( ledService.uuid() );

  // add service and characteristics
  blePeripheral.addAttribute( ledService );
  blePeripheral.addAttribute( ledCharacteristic );
  ledCharacteristic.setValue( 0 );

  // advertise the service
  blePeripheral.begin();

  Serial.println("Bluetooth device active, waiting for connections...");
  
}


void loop() {
  
  // poll peripheral
  blePeripheral.poll();

  BLECentral central = blePeripheral.central();

  if ( central && !addressShown ) {
    addressShown = true;
    Serial.print( "Connected to central: " );
    Serial.println(central.address());
  }

  if( ledCharacteristic.written() ){
    Serial.print("LED Characteristic = ");
    Serial.println( ledCharacteristic.value() );
    
    ledValue = !ledValue;
    if( ledValue == true ){
      digitalWrite( 13, HIGH );
    }else {
      digitalWrite( 13, LOW );
    }
  }
    
}


