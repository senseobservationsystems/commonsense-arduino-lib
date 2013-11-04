#ifndef COMMONSENSE_H
#define COMMONSENSE_H

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

/**
* @class	CommonSense
*
* @brief	Class containing variables and functions required for interacting with CommonSense.
*
* @author	Miguel
* @date	01/10/2013
*/

class CommonSense
{
public:
	/**
	* @fn	CommonSense::CommonSense(const char *username, const char *password,
	* const char *deviceName, const byte *MAC)
	*
	* @brief	Constructor used to provide essential information.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @param	username  	Your CommonSense username.
	* @param	password  	MD5 hash of your password.
	* @param	deviceName	Name of the device.
	* @param	MAC		  	The MAC address of your Arduino.
	*/
	CommonSense(const char *username, const char *password, const char *deviceName, const byte *MAC);
	/**
	* @fn	CommonSense::CommonSense(const char *username, const char *password,
	* const byte *MAC)
	*
	* @brief	Constructor used to provide essential information.
	*
	* @author	Miguel
	* @date	04/11/2013
	*
	* @param	username  	Your CommonSense username.
	* @param	password  	MD5 hash of your password.
	* @param	MAC		  	The MAC address of your Arduino.
	*/
	CommonSense(const char *username, const char *password, const byte *MAC);

	/**
	* @brief	Digit used to set the amount of printing done to the serial monitor.
	* 0 = no printing
	* 1 = status codes and error messages
	* 2 = full responses
	*/
	uint8_t debug;

	/**
	* @fn	int8_t CommonSense::begin()
	*
	* @brief	Sets up ethernet, time and calls CommonSense::login().
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @return	1 if successful.
	*/
	int8_t begin();

	/**
	* @fn	int8_t CommonSense::createSensor(char *displayName, char *deviceType, char *name, int pin,
	* int32_t *sensorArray, size_t arraySize);
	*
	* @brief	Creates a sensor on CommonSense and attempts to store the sensor id and mapped pin in sensorArray and EEPROM.
	*
	* @author	Miguel
	* @date	24/10/2013
	*
	* @param 	displayName	The "Name" displayed in the sensor library on CommonSense.
	* @param 	deviceType 	The description displayed in the sensor library.
	* @param 	name	   	Internal name not visible in the sensor library.
	* @param	pin			   	The pin this sensor is supposed to read.
	* @param 	sensorArray	The sensor array where the id and pin are stored (for burning to
	* EEPROM).
	* @param	arraySize	   	Amount of elements in sensorArray.
	*
	* @return	1 if the id and pin are successfully stored in the sensorArray and EEPROM.
	*/
	int8_t createSensor(char *displayName, char *deviceType, char *name, int8_t pin, int32_t *sensorArray, size_t arraySize);
	/**
	* @fn	int8_t CommonSense::deleteSensor(uint8_t elementNo, int32_t* sensorArray, size_t arraySize);
	*
	* @brief	Deletes the sensor stored at elementNo in sensorArray from 
	* CommonSense and erases it from EEPROM.
	*
	* @author	Miguel
	* @date	24/10/2013
	*
	* @param elementNo	Number of the element (sensor id) to be removed.
	* @param 	sensorArray	The sensor array where the id and pin are stored.
	* @param arraySize Amount of elements in sensorArray.
	*
	* @return	1 if the sensor was successfully deleted from CommonSense and EEPROM.
	*/
	int8_t deleteSensor(uint8_t elementNo, int32_t* sensorArray, size_t arraySize);

	/**
	* @fn	int8_t CommonSense::uploadData(const int32_t sensorId, const int value, uint32_t time);
	*
	* @brief	Uploads a value for this data point using the current time.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @param	sensorId	Identifier of the sensor to store data in.
	* @param	value   	The value for this data point.
	* @param	time		The unix timestamp of this data point.
	*
	* @return	1 if uploading the data was successful.
	*/
	int8_t uploadData(const int32_t sensorId, const int16_t value, uint32_t time); 
	/**
	* @fn	int8_t CommonSense::uploadDigitalData(int32_t *sensorArrayElement);
	*
	* @brief	Uploads data from a digital pin using the sensorArrayElement and time().
	*
	* @author	Miguel
	* @date	24/10/2013
	*
	* @param 	sensorArrayElement	Pointer to the sensor id in the array.
	*
	* @return	1 if uploading the data was successful.
	*/
	int8_t uploadDigitalData(int32_t *sensorArrayElement);
	/**
	* @fn	int8_t CommonSense::uploadAnalogData(int32_t *sensorArrayElement);
	*
	* @brief	Uploads data from an analog pin using the sensorArrayElement and time().
	*
	* @author	Miguel
	* @date	24/10/2013
	*
	* @param 	sensorArrayElement	Pointer to the sensor id in the array.
	*
	* @return	1 if uploading the data was successful.
	*/
	int8_t uploadAnalogData(int32_t *sensorArrayElement);
	/**
	* @fn	int16_t CommonSense::getLastData(const int32_t sensorId);
	*
	* @brief	Gets the latest data value for this sensor.
	*
	* @author	Miguel
	* @date	04/10/2013
	*
	* @param	sensorId	Identifier of the sensor to get the data from.
	*
	* @return	The last data value.
	*/
	int16_t getLastData(const int32_t sensorId);

	/**
	* @fn	int8_t CommonSense::saveSensorsToEEPROM(int32_t *sensorArray, size_t arraySize);
	*
	* @brief	Saves the sensor array to EEPROM.
	*
	* @author	Miguel
	* @date	24/10/2013
	*
	* @param 	sensorArray	The SRAM array to be written to EEPROM.
	* @param	arraySize		   	Size of the array.
	*
	* @return	1 if successfully wrote the array to EEPROM.
	*/
	int8_t saveSensorsToEEPROM(int32_t *sensorArray, size_t arraySize);
	/**
	* @fn	int8_t CommonSense::loadSensorsFromEEPROM(int32_t *sensorArray, size_t arraySize);
	*
	* @brief	Loads sensors saved in EEPROM into the sensor array.
	*
	* @author	Miguel
	* @date	24/10/2013
	*
	* @param [out]	sensorArray	The SRAM array to write the EEPROM array to.
	* @param	arraySize		   	Size of the array.
	*
	* @return	The sensors from eeprom.
	*/
	int8_t loadSensorsFromEEPROM(int32_t *sensorArray, size_t arraySize);
	/**
	* @fn	int8_t CommonSense::clearEEPROM();
	*
	* @brief	Clears the EEPROM.
	*
	* @author	Miguel
	* @date	24/10/2013
	*
	* @return	1 if successfully erased all bytes of the EEPROM.
	*/
	int8_t clearEEPROM();

	/**
	* @fn	bool CommonSense::EEPROMContainsPin(uint8_t pin);
	*
	* @brief	Checks if the EEPROM already contains pin.
	*
	* @author	Miguel
	* @date	24/10/2013
	*
	* @return	true if pin was found in EEPROM.
	*/
	bool EEPROMContainsPin(int8_t pin);

	/**
	* @fn	uint32_t CommonSense::time();
	*
	* @brief	Gets the current unix time. This format is used for dates in the CommonSense platform.
	*
	* @author	Miguel
	* @date	16/10/2013
	*
	* @return	The amount of seconds since 00:00:00 UTC, 1 January 1970
	*/
	uint32_t time();

	/**
	* @fn	const char* CommonSense::sessionId() const
	*
	* @brief	Gets the sessionId_ in this object.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @return	A copy of the sessionId_ in this object.
	*/
	const char* sessionId() const;

protected:
private:

	/**
	* @brief	The username used to logon to CommonSense.
	*/

	const char *username_;

	/**
	* @brief	The password belonging to username_.
	*/

	const char *password_;

	/**
	* @brief	A name used to group sensors on CommonSense.
	*/

	const char *deviceName_;

	/**
	* @brief	The MAC address belonging to the Ethernet shield.
	*/

	const byte *MAC_;

	/**
	* @brief	Storage for the session_id received from CommonSense.
	*/

	char sessionId_[28];

	EthernetUDP Udp;
	IPAddress timeServer;

	const uint8_t NTP_PACKET_SIZE;

	uint32_t timeSinceStart;
	uint32_t NTPTime;



	/**
	* @fn	int CommonSense::login()
	*
	* @brief	Log in using the credentials stored in this object.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @return	The response status code from the API. In case of connection failure, 1 is returned.
	*/
	int16_t login();

	void* parseResponse(uint8_t request, int16_t* status, EthernetClient &client);

	/**
	* @fn	void CommonSense::writeToClient(const __FlashStringHelper *str, EthernetClient &client)
	*
	* @brief	Writes a F() string to client. This should be more efficient than using
	* client.print() functions due to those sending a single byte per packet.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @param	str		  	The flash string to be written to the client.
	* @param 	client	The client.
	*/
	void writeToClient(const __FlashStringHelper *str, EthernetClient &client);
	/**
	* @fn	void CommonSense::writeToClient(char *str, EthernetClient &client)
	*
	* @brief	Writes a char* to client. This should be more efficient than using client.print()
	* functions due to those sending a single byte per packet.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @param 	str   	The char* to be written to the client.
	* @param 	client	The client.
	*/
	void writeToClient(char *str, EthernetClient &client);
	void writeHeadersToClient(EthernetClient &client, size_t length, bool sessionRequired);


	/**
	* @fn	int setupTime()
	*
	* @brief	Sets up the UDP connection and calls setTime(getNtpTime).
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @return	The current time after setup.
	*/
	int8_t setupTime();

	/**
	* @fn	void sendNTPpacket(IPAddress &address)
	*
	* @brief	Sends a NTP packet containing a NTP request.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @param 	address	The IP address of a NTP server.
	*/
	void sendNTPpacket(IPAddress &address);

	/**
	* @fn	time_t getNtpTime()
	*
	* @brief	Gets the current NTP time by using the sendNTPpacket() function.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @return	The synchronised time adjusted to Unix epoch.
	*/
	uint32_t getNtpTime();

	bool hasSpaceLeft(int32_t* arr, size_t arraySize);
};

static const char server[] = "api.sense-os.nl";
extern int32_t EEMEM dataInEEPROM[];

//define eeprom size for this board
#if defined(__AVR_ATmega168__)
#define EEPROM_MAX 512
#elif defined(__AVR_ATmega328P__)
#define EEPROM_MAX 1024
#elif defined(__AVR_ATmega1280__)
#define EEPROM_MAX 4096
#elif defined(__AVR_ATmega2560__)
#define EEPROM_MAX 4096
#elif defined(__AVR_ATmega32U4__)
#define EEPROM_MAX 4096
#elif defined(__SAM3X8E__)
#error SAM3X8E does not have EEPROM
#endif

#endif