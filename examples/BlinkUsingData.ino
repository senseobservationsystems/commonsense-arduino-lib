/*
In this example, we will be doing the following things:

- Logging in
- Creating sensors
- Uploading data
- Getting the latest data
- Light up an LED based on the latest data
- Delete the sensor after repeating upload and retrieval process 5 times

***Setting up user variables***
Start with filling in the variables username, password and MAC (deviceName is not used yet).

Feel free to adjust the delay_ between loop() iterations or the pin your LED is connected to.

***Setting up the circuit***

This example was made using an Arduino Uno R3. While this board has a LED and resistor attached to pin 13, using the Ethernet Shield disables the use of them.

The circuit required for this example consists of a 220-ohm resistor, an LED and the Arduino.

Connect the short (negative) leg to ground (GND) and the long (positive) leg to the resistor.
Attach the resistor to digital pin 2.

Your circuit should look like this:

Digital Pin 2 - Resistor - LED - Ground

Connecting the wrong legs shouldn't damage the LED as long as you use a resistor.
If the LED doesn't light up, you probably connected it the wrong way.
*/

#include <SPI.h> // Library used by the CommonSense library
#include <Ethernet.h> // Library used by the CommonSense library 
#include <CommonSense.h> // Our library itself

//user vars
const char *username = ""; //Required: Your CommonSense username
const char *password = "";//Required: MD5 hash of your CommonSense account password
const char *deviceName = "ArduinoUno"; //Required: A name for this device in CommonSense
const byte MAC[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }; //Required: Your MAC address

const int delay_ = 5; //delay between upload requests

long led; //sensor id
int ledPin = 2; // the led is connected to pin 2

CommonSense sense = CommonSense(username,password,deviceName,MAC);

void setup(){
	Serial.begin(9600);
	Serial.print(F("Hello world!"));

	pinMode(ledPin, OUTPUT);

	sense.begin();

	led = sense.createSensor("BlinkingLight", "LED", "arduinoLed2");
	sense.uploadData(led, 0, sense.time());
}

int cycle = 0;
void loop(){
	Serial.print("Cycle: ");
	Serial.println(cycle);
	Serial.print(F("Time: "));
	Serial.println(sense.time());

	int lastData = sense.getLastData(led);

	if(lastData == 0){
		sense.uploadData(led, 1, sense.time());
		digitalWrite(ledPin, HIGH);
		delay(1000 * delay_);
	}else{
		sense.uploadData(led, 0, sense.time());
		digitalWrite(ledPin, LOW);
		delay(1000 * delay_);
	}

	Serial.println();
	cycle++;
	if(10 == cycle){
		sense.deleteSensor(led);
		while(true) ; //blinked 100 times
	}
}
