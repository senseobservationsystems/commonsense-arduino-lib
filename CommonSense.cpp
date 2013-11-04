#include "CommonSense.h"

CommonSense::CommonSense(const char *username, const char *password, const char *deviceName, const byte *MAC) : username_(username), password_(password), deviceName_(deviceName), MAC_(MAC), timeServer(132, 163, 4, 101), NTP_PACKET_SIZE(48), debug(0){ 

}

CommonSense::CommonSense(const char *username, const char *password, const byte *MAC) : username_(username), password_(password), MAC_(MAC), timeServer(132, 163, 4, 101), NTP_PACKET_SIZE(48), debug(0){ 

}

int8_t CommonSense::begin(){
	Ethernet.begin(const_cast<byte *>(MAC_));
	if(debug > 0){Serial.print(F("My IP is:"));
	Serial.println(Ethernet.localIP());}

	setupTime();

	int16_t response = login();

	if(200 == response){
		return 1;
	}else{
		if(debug > 0){Serial.println(F("Retrying..."));}

		response = login();

		if(200 == response){
			return 1;
		}else if(11 == response){
			return 11;
		}else{
			if(debug > 0){Serial.println(F("CommonSense::begin() failed!"));}
			return 12;
		}
	}
	return -1;
}

int16_t CommonSense::login(){
	if(debug > 0){Serial.println(F("login()..."));}

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){
		char line[128] = {'\0'};
		char chrIn[2] = {'\0'};
		int16_t status = 1;

		sprintf_P(line,PSTR("{\"username\":\"%s\",\"password\":\"%s\"}"),username_,password_);

		writeToClient(F("POST /login.json HTTP/1.1\r\n"), client);
		writeHeadersToClient(client,strlen(line),false);
		writeToClient(line, client);		

		memset(line,0,sizeof(line));

		strcpy(sessionId_, static_cast<char *>(parseResponse(0, &status, client)));
		return status;

	}else{
		if(debug > 0){Serial.println(F("client.connect() failed!"));}
		client.stop();
		return 11;
	}

	client.stop();
	return -1;
}

int8_t CommonSense::createSensor(char *displayName, char *deviceType, char *name, int8_t pin, int32_t *sensorArray, size_t arraySize){
	int32_t newSensorId = 0;
	int8_t newPin = pin;
	int8_t newSpace = -1;

	if(debug > 0){Serial.println(F("createSensor()..."));}

	if(EEPROMContainsPin(pin)){
		if(debug > 0){Serial.println(F("Sensor with this pin already exists."));}
		return 1;
	}

	if(!hasSpaceLeft(sensorArray, arraySize)){
		if(debug > 0){Serial.println(F("No space left in array for new sensor!\nStopping..."));}
		while(true) ;
		return 22;
	}

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){ 
		char line[256] = {'\0'};
		char chrIn[2] = {'\0'};
		int16_t status = 1;

		sprintf_P(line,PSTR("{\"sensor\":{\"display_name\":\"%s\",\"device_type\":\"%s\",\"data_type\":\"json\",\"name\":\"%s\",\"data_structure\":\"{\\\"value\\\":\\\"Integer\\\"}\"}}"),displayName,deviceType,name);

		writeToClient(F("POST /sensors.json HTTP/1.1\r\n"), client);
		writeHeadersToClient(client,strlen(line),true);
		writeToClient(line, client);

		memset(line,0,sizeof(line));

		newSensorId = *(static_cast<int32_t *>(parseResponse(1, &status, client)));
		client.stop();
	}else{
		if(debug > 0){Serial.println(F("client.connect() failed!"));}
		client.stop();
		return 11;
	}

	for(uint16_t i = 0; i < arraySize - 1; i++){
		if(0 == *(sensorArray+i) && 0 == *(sensorArray+i+1)){
			newSpace = i;
			*(sensorArray+i) = newSensorId;
			*(sensorArray+i+1) = newPin;
			if(0 != debug){
				Serial.print(F("sensors["));
				Serial.print(i);
				Serial.print(F("] is now: "));
				Serial.println(*(sensorArray+i));
				Serial.print(F("sensors["));
				Serial.print((int8_t) (i+1));
				Serial.print(F("] is now: "));
				Serial.println(*(sensorArray+i+1));
			}

			saveSensorsToEEPROM(sensorArray, arraySize);

			int32_t result[arraySize];
			loadSensorsFromEEPROM(result, arraySize);
			if(0 == memcmp(result, sensorArray, sizeof(result))){
				client.stop();
				return 1;
			}else{
				if(0 != debug){
					Serial.println(F("EEPROM sensors != SRAM sensors"));
					Serial.print(F("id(EEPROM):"));
					Serial.println(result[newSpace]);
					Serial.print(F("id(SRAM):"));
					Serial.println(*(sensorArray));
					Serial.print(F("pin(EEPROM):"));
					Serial.println(result[newSpace+1]);
					Serial.print(F("pin(SRAM):"));
					Serial.println(*(sensorArray+1));
				}
				client.stop();
				return 21;
			}

		}
	}

	client.stop();
	return -1; 
}

int8_t CommonSense::deleteSensor(uint8_t elementNo, int32_t* sensorArray, size_t arraySize){
	if(debug > 0){Serial.println(F("deleteSensor()..."));}

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){
		char line[256] = {'\0'};
		char chrIn[2] = {'\0'};
		int16_t status = 1;

		sprintf_P(line,PSTR("DELETE /sensors/%ld HTTP/1.1\r\n"),*(sensorArray+elementNo));
		writeToClient(line, client);
		writeHeadersToClient(client,0,true);

		memset(line,0,sizeof(line));

		status = *static_cast<int16_t *>(parseResponse(2, &status, client));

		if(200 == status){
			*(sensorArray+elementNo) = 0;
			*(sensorArray+elementNo+1) = 0;
			saveSensorsToEEPROM(sensorArray, arraySize);

			return 1;
		}

	}else{
		if(debug > 0){Serial.println(F("client.connect() failed!"));}
		client.stop();
		return 10;
	}

	client.stop();
	return -1;
}

int8_t CommonSense::uploadData(const int32_t sensorId, const int16_t value, uint32_t time){
	if(debug > 0){Serial.println(F("uploadData()..."));}

	if(0 >= sensorId){
		Serial.println(F("Sensor id is 0!"));
		return 31;
	}

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){
		char line[256] = {'\0'};
		char chrIn[2] = {'\0'};
		int16_t status = -1;

		sprintf_P(line,PSTR("POST /sensors/%ld/data.json HTTP/1.1\r\n"),sensorId);
		writeToClient(line, client);
		memset(line,0,sizeof(line));
		sprintf_P(line, PSTR("{\"data\":[{\"value\":%d,\"date\":%lu}]}"),value, time);
		writeHeadersToClient(client,strlen(line),true);
		writeToClient(line, client);

		memset(line,0,sizeof(line));

		parseResponse(3, &status, client);
		if(200 == status){
			client.stop();
			return 1;
		}

	}else{
		if(debug > 0){Serial.println(F("client.connect() failed!"));}
		client.stop();
		return 10;
	}

	client.stop();
	return -1;
}

int8_t CommonSense::uploadDigitalData(int32_t *sensorArrayElement){
	int32_t id = *sensorArrayElement;
	int32_t pin = *(sensorArrayElement+1);
	int16_t data = digitalRead(pin);
	uint32_t time = CommonSense::time();

	return uploadData(id, data, time);
}

int8_t CommonSense::uploadAnalogData(int32_t *sensorArrayElement){
	int32_t id = *sensorArrayElement;
	int32_t pin = *(sensorArrayElement+1);
	int16_t data = analogRead(pin);
	uint32_t time = CommonSense::time();

	return uploadData(id, data, time);
}

int16_t CommonSense::getLastData(const int32_t sensorId){
	if(debug > 0){Serial.println(F("getLastData()..."));}

	int16_t status;
	int16_t data;
	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){
		char line[256] = {'\0'};
		char chrIn[2] = {'\0'};

		sprintf_P(line,PSTR("GET /sensors/%ld/data?last=true HTTP/1.1\r\n"),sensorId);
		writeToClient(line, client);
		writeHeadersToClient(client,0,true);

		memset(line,0,sizeof(line));

		data = *static_cast<int16_t *>(parseResponse(4, &status, client));
		return data;

	}else{
		if(debug > 0){Serial.println(F("client.connect() failed!"));}
		client.stop();
		return 11;
	}

	client.stop();
	if(debug > 0){Serial.println(F("Something went wrong in getLastData()..."));}
	return -1;
}

int8_t CommonSense::saveSensorsToEEPROM(int32_t *sensorArray, size_t arraySize){
	int32_t temp1[arraySize];
	int32_t temp2[arraySize];

	memcpy(temp1, sensorArray, sizeof(temp1));
	eeprom_read_block(temp2,dataInEEPROM,sizeof(temp2));

	if(0 == memcmp(temp1, temp2, sizeof(temp1))){
		if(0 != debug)
			Serial.println(F("Supplied data is already in EEPROM!"));
		return 1;
	}else{
		eeprom_write_block(temp1, dataInEEPROM, sizeof(temp1));

		eeprom_read_block(temp2, dataInEEPROM, sizeof(temp2));
		if(0 == memcmp(temp1, temp2, sizeof(temp1))){
			if(0 != debug)
				Serial.println(F("Successfully copied data from RAM to EEPROM"));
			return 1;
		}else{
			if(0 != debug)
				Serial.println(F("Copied data does not match original data!"));
			if(2 == debug){
				for(uint16_t i = 0; i < arraySize; i++){
					Serial.print(F("RAM["));Serial.print(i);Serial.print(F("]: "));Serial.println(*(sensorArray+i));
					Serial.print(F("EEPROM["));Serial.print(i);Serial.print(F("]: "));Serial.println(temp2[i]);
				}
			}
			return 21;
		}
	}

	return -1;
}

int8_t CommonSense::loadSensorsFromEEPROM(int32_t *sensorArray, size_t arraySize){
	int32_t temp1[arraySize];

	eeprom_read_block(temp1,dataInEEPROM,sizeof(temp1));

	memcpy(sensorArray,temp1,sizeof(temp1));

	if(0 != debug){
		for(uint16_t i = 0;i<arraySize;i++){
			if(0 == i%2){
				if(0 != *(sensorArray+i)){
					Serial.print(F("Loaded from EEPROM: "));
					Serial.print(*(sensorArray+i));
					Serial.print(F(" | "));
				}
			}
			else{
				if(0 != *(sensorArray+i))
					Serial.println((int8_t)*(sensorArray+i));
			}
		}

	}
	return 1;
}

int8_t CommonSense::clearEEPROM(){
	uint16_t e = 0;
	uint16_t s = 0;

	for(uint16_t i = 0; i < EEPROM_MAX; i++){
		if(0 == eeprom_read_byte((uint8_t*)i)){
			s++;
		}
		else{
			eeprom_write_byte((uint8_t*)i, 0);
			e++;
		}
	}
	if(debug > 0){
		Serial.print(F("Erased: "));
		Serial.println(e);
		Serial.print(F("Skipped: "));
		Serial.println(s);
	}
	return 1;
}

bool CommonSense::EEPROMContainsPin(int8_t pin){
	for(uint16_t i = 0; i < EEPROM_MAX; i=i+4){
		int8_t t = eeprom_read_dword((uint32_t*)i);
		if(pin == t){
			if(debug > 0){Serial.print(F("Found: "));
			Serial.println(t);}
			return true;
		}
	}
	if(debug > 0){Serial.print(F("Did not find pin "));
	Serial.println(pin);}
	return false;
}

void CommonSense::writeToClient(const __FlashStringHelper *str, EthernetClient &client){ // http://forum.arduino.cc/index.php?topic=90846.msg687391#msg687391
	char * p = (char *) str;
	if (!p) return;  // no string, uh oh!

	char buf [strlen_P (p) + 1];  // dynamic allocation ;)
	byte i = 0;
	char c;
	while ((c = pgm_read_byte(p++)))
		buf [i++] = c;
	buf [i] = 0;

	client.write((byte*)buf,strlen(buf));
}

void CommonSense::writeToClient(char *str, EthernetClient &client){
	client.write((byte*)str,strlen(str));
}

const char* CommonSense::sessionId() const{
	return sessionId_;
}

void CommonSense::writeHeadersToClient(EthernetClient &client, size_t length, bool sessionRequired){
	writeToClient(F("Host: api.sense-os.nl\r\n"), client);
	writeToClient(F("X-TARGET-URI: http://api.sense-os.nl\r\n"), client);
	if(sessionRequired){
		writeToClient(F("X-SESSION_ID: "), client);writeToClient(sessionId_, client);writeToClient(F("\r\n"), client);
	}
	if(0 != length){
		writeToClient(F("Content-Type: application/json; charset=UTF-8\r\n"), client);
		writeToClient(F("Content-Length: "), client);client.print(length);writeToClient(F("\r\n"), client);
	}
	writeToClient(F("Accept: *\r\n"), client);
	writeToClient(F("Connection: keep-alive\r\n"), client);
	writeToClient(F("\r\n"), client);
}

void* CommonSense::parseResponse(uint8_t request, int16_t* status, EthernetClient &client){
	/*
	request codes: 
	0 - login
	1 - create sensor
	2 - delete sensor
	3 - upload data
	4 - get latest data (int)
	*/
	const char *str[] = {"X-SESSION_ID","Location:","value"};
	int16_t err = -1;
	char line[256] = {'\0'};
	char input[2] = {'\0'};

	while(client.connected()){//Do something while we are connected
		if(client.available()){//Do we have a byte? if so...
			if(debug == 2){
				Serial.print(input[0] = client.read());
			}else{
				input[0] = client.read();
			}
			if('\r' != input[0] || '\n' == input[0])
				strncat(line, input, 1);
			if('\r' == input[0]){//We're done reading a line...
				if(NULL != strstr_P(line, PSTR("HTTP/1.1"))){ //Does the line we just read contain "HTTP/1.1"?
					char *ptr;
					ptr = strtok(line, " ");
					ptr = strtok(NULL, " ");

					//printf("parseResponse:Received status: %s\n", ptr);
					if(debug > 0){Serial.print(F("parseResponse:Received status: "));
					Serial.println(ptr);}

					*status = atoi(ptr);
					err = atoi(ptr);

					if(request == 2 || request == 3){//request != 2 && 
						client.stop();
						return &err;
					}
				}

				switch(request){
				case 0:
					if(NULL != strstr(line,str[0])){
						char *ptr;
						ptr = strtok(line, ":");
						ptr = strtok(NULL, " ");

						char data[28];
						strncpy(data,ptr,sizeof(data));
						if(debug > 0){Serial.print(F("parseResponse:Received session_id: "));
						Serial.println(data);}
						client.stop();
						return data;
					}
				case 1:
					if(NULL != strstr(line,"Location:")){//Is this the line containing "Location"?
						char *ptr;
						ptr = strtok(line, " ");
						ptr = strtok(NULL, "/");
						ptr = strtok(NULL, "/");
						ptr = strtok(NULL, "/");
						ptr = strtok(NULL, "/");

						int32_t data = atol(ptr);
						if(debug > 0){Serial.print(F("parseResponse:Received sensor_id: "));
						Serial.println(data);}
						client.stop();
						return &data;
					}
				case 4:
					if(NULL != strstr(line, "value")){
						char *t;
						char *ptr;

						t = strstr(line, "value");
						ptr = strtok(t, "\"");
						ptr = strtok(NULL, "\"");
						ptr = strtok(NULL, "\"");

						int16_t data = atoi(ptr);
						if(debug > 0){Serial.print(F("parseResponse:Received value: "));
						Serial.println(data);}
						client.stop();
						return &data;
					}
				default:
					//Serial.print(F("Not the line we're looking for..."));
					break;
				}

				memset(line,0,sizeof(line));
			}

		}else{
			//no byte available to read...
		}
	}
	client.stop();
	return &err;
}

uint32_t CommonSense::time(){
	return NTPTime + (millis()/1000) - timeSinceStart;
}

int8_t CommonSense::setupTime(){
	Udp.begin(8888u);

	NTPTime = getNtpTime();
	timeSinceStart = (millis() / 1000);
	Udp.stop();
	return 1;
}

uint32_t CommonSense::getNtpTime() //from UdpNtpClient example
{
	byte packetBuffer[NTP_PACKET_SIZE];
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	sendNTPpacket(timeServer);

	delay(1000);

	if (Udp.parsePacket()) {
		Udp.read(packetBuffer, NTP_PACKET_SIZE);

		uint32_t highWord = word(packetBuffer[40], packetBuffer[41]);
		uint32_t lowWord = word(packetBuffer[42], packetBuffer[43]);

		uint32_t secsSince1900 = highWord << 16 | lowWord;

		return secsSince1900 - 2208988800UL;// return current epoch
	}

	return 0; //return if unsuccessful
}

void CommonSense::sendNTPpacket(IPAddress &address)// From Time example TimeNTP
	// send an NTP request to the time server at the given address
{
	byte packetBuffer[NTP_PACKET_SIZE];
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:                 
	Udp.beginPacket(address, 123); //NTP requests are to port 123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
}

bool CommonSense::hasSpaceLeft(int32_t* arr, size_t arraySize){
	for(uint16_t i = 0; i < arraySize-1; i++){
		if(*(arr+i) == 0 && *(arr+i+1) == 0)
			return true;
	}
	return false;
}