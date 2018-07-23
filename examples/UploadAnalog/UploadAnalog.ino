//LDR Example

#include <SPI.h> // Library used by the CommonSense library
#include <Ethernet.h> // Library used by the CommonSense library 
#include <CommonSense.h> // Our library itself

//user variables
const char *username = ""; // your CommonSense username
const char *password = ""; // MD5 hash of your password
const byte MAC[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }; //mac address used to setup ethernet

CommonSense sense = CommonSense(username,password,MAC); // object we use to talk to commonsense

long sensors[10]; // array containing the pin and its sensor id
int sensorsSize = sizeof(sensors) / sizeof(sensors[0]); // the amount of elements in sensors[]
long EEMEM dataInEEPROM[sizeof(sensors) / sizeof(sensors[0])]; // allocate space in EEPROM for a long array the size of sensors[]

unsigned long timer; // store the current time so we can do something in x seconds

void setup(){
	Serial.begin(9600);
	Serial.println(F("Hello world!"));

	sense.debug = 1; // print all the details to the serial monitor

	sense.begin(); //setup ethernet, time and logon to commonsense

	sense.loadSensorsFromEEPROM(sensors, sensorsSize); // load the sensors from the eeprom into sensors[]

	sense.createSensor("Light sensor", "LDR", "arduinoldr", A0, sensors, sensorsSize); // create a sensor on commonsense

	timer = millis(); // set the timer to the current time
}

void loop(){

	if(millis() - timer >= 5000UL){
		timer = millis();
		Serial.print(F("Time: "));
		Serial.println(sense.time()); // print the current unix time

		sense.uploadAnalogData(&sensors[0]); // read the sensor in sensor[0] and upload it with the current time as timestamp
	}
}
