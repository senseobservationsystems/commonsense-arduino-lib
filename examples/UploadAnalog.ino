#include <Arduino.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <CommonSense.h>

//user variables
const char *username = ""; //Required: Your CommonSense username
const char *password = "";//Required: MD5 hash of your CommonSense account password
const char *deviceName = "ArduinoUno"; //Not necessary (yet)
const byte MAC[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }; //Required: Your MAC address

const int delay_ = 10; //delay between loop()

long banaan; //save the sensor id globally

CommonSense sense = CommonSense(username,password,deviceName,MAC);

void setup(){
	Serial.begin(9600);
	Serial.print(F("Hello world!"));

	sense.begin();

	banaan = sense.createSensor("Banaan", "Fruit", "analoogFruit");
}

void loop(){
	Serial.print(F("Time: "));
	Serial.println(sense.time());

	sense.uploadData(banaan,analogRead(0), sense.now());
	//sense.deleteSensor(banaan); // Deleting a sensor will also delete all it's data!

	Serial.println();
	delay(1000 * delay_);
}
