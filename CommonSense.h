#ifndef COMMONSENSE_H
#define COMMONSENSE_H

#include <avr/pgmspace.h>
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
	* @fn	int CommonSense::begin()
	*
	* @brief	Sets up Ethernet, Time and calls CommonSense::login().
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @return	The response code from CommonSense::login().
	*/
	int begin();

	/**
	* @fn	long CommonSense::createSensor(char *displayName, char *deviceType, char *name);
	*
	* @brief	Creates a sensor using the provided parameters.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @param [in]	displayName	The "Name" displayed in the sensor library on CommonSense.
	* @param [in]	deviceType 	The description displayed in the sensor library.
	* @param [in]	name	   	Internal name no visible in the sensor library.
	*
	* @return	The new sensor id if created successfully or the response status. In case of
	* connection failure, 1 is returned.
	*/

	long createSensor(char *displayName, char *deviceType, char *name);

	/**
	 * @fn	int CommonSense::uploadData(const long sensorId, const int value, unsigned long time);
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
	 * @return	The response status code from the API. In case of connection failure, 1 is returned.
	 */

	int uploadData(const long sensorId, const int value, unsigned long time); 

	/**
	* @fn	int CommonSense::getLastData(const long sensorId);
	*
	* @brief	Gets the latest data value for this sensor.
	*
	* @author	Miguel
	* @date	04/10/2013
	*
	* @param	sensorId	Identifier of the sensor to get the data from.
	*
	* @return	The last data.
	*/

	int getLastData(const long sensorId);

	/**
	* @fn	int CommonSense::deleteSensor(long sensorId)
	*
	* @brief	Deletes the sensor with sensorId.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @param	sensorId	Identifier for the sensor.
	*
	* @return	The response status code from the API. In case of connection failure, 1 is returned.
	*/
	int deleteSensor(long sensorId);

	/**
	 * @fn	unsigned long CommonSense::time();
	 *
	 * @brief	Gets the current unix time. This format is used for dates in the CommonSense platform.
	 *
	 * @author	Miguel
	 * @date	16/10/2013
	 *
	 * @return	The amount of seconds since 00:00:00 UTC, 1 January 1970
	 */

	unsigned long time();

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

	const int NTP_PACKET_SIZE;

	unsigned long timeSinceStart;
	unsigned long NTPTime;

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
	int login();

	void* parseResponse(int request, int* status, EthernetClient &client);

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
	* @param [in]	client	The client.
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
	* @param [in]	str   	The char* to be written to the client.
	* @param [in]	client	The client.
	*/
	void writeToClient(char *str, EthernetClient &client);

	int writeHeadersToClient(EthernetClient &client, int length, bool sessionRequired);


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
	int setupTime();

	/**
	* @fn	void sendNTPpacket(IPAddress &address)
	*
	* @brief	Sends a NTP packet containing a NTP request.
	*
	* @author	Miguel
	* @date	01/10/2013
	*
	* @param [in]	address	The IP address of a NTP server.
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
	unsigned long getNtpTime();
};

static const char server[] = "api.sense-os.nl";

#endif
