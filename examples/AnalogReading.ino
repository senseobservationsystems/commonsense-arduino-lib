#include <Arduino.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Time.h>
#include <CommonSense.h>

//user vars
const char *username = ""; //Required: Your CommonSense username
const char *password = "";//Required: MD5 hash of your CommonSense account password
const char *deviceName = "ArduinoUno"; //Required: A name for this device in CommonSense
const byte MAC[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }; //Required: Your MAC address

const int delay_ = 3; //delay between upload requests

long banaan;

CommonSense sense = CommonSense(username,password,deviceName,MAC);

void setup(){
	Serial.begin(9600);
	Serial.print(F("Hello world!"));

	sense.begin();

	banaan = sense.createSensor("Banaan", "Fruit", "analoogFruit");
}

void loop(){
	Serial.print(F("Time: "));
	Serial.println(now());

	sense.uploadData(banaan,analogRead(0));
	//sense.deleteSensor(banaan); // Deleting a sensor will also delete all it's data!

	Serial.println();
	delay(1000 * delay_);
}
