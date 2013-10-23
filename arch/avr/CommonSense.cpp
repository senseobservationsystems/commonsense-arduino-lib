#include "CommonSense.h"

CommonSense::CommonSense(const char *username, const char *password, const char *deviceName, const byte *MAC) : username_(username), password_(password), deviceName_(deviceName), MAC_(MAC), timeServer(132, 163, 4, 101), NTP_PACKET_SIZE(48){ 

}

int CommonSense::begin(){
	Ethernet.begin(const_cast<byte *>(MAC_));
	delay(1000);
	Serial.print(F("\nMy IP is:"));
	Serial.println(Ethernet.localIP());

	setupTime();

	int response = login();
	switch(response){
	case 200:
		return response;
		break;
	default:
		delay(1000);
		Serial.println(F("Retrying..."));
		response = login();
		if(200 != response){
			Serial.println("CommonSense::begin() failed!");
			return(response);
		}
	}

	return 1;
}

int CommonSense::login(){
	Serial.println(F("login()..."));

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){
		char line[128] = {'\0'};
		char chrIn[2] = {'\0'};
		int status = 1;

		sprintf_P(line,PSTR("{\"username\":\"%s\",\"password\":\"%s\"}"),username_,password_);

		writeToClient(F("POST /login.json HTTP/1.1\r\n"), client);
		writeHeadersToClient(client,strlen(line),false);
		writeToClient(line, client);		

		memset(line,0,sizeof(line));

		strcpy(sessionId_, static_cast<char *>(parseResponse(0, &status, client)));
		return status;

		/*while(client.connected()){//While we are still connected...
		if(client.available()){//Are there bytes available to read?
		//Serial.print(chrIn[0] = client.read()); //Want to see what the response looks like?
		chrIn[0] = client.read();
		if('\r' != chrIn[0] || '\n' == chrIn[0])// Keep reading bytes into the buffer until the CRLF
		strncat(line, chrIn, 1);
		if('\r' == chrIn[0]){// Done reading a line, process it...

		if(NULL != strstr_P(line,PSTR("HTTP/1.1"))){ //Does the line we just read contain "HTTP/1.1"?
		char *ptr;
		ptr = strtok(line, " ");
		ptr = strtok(NULL, " ");

		Serial.print(F("Received status: "));
		Serial.println(ptr);
		status = atoi(ptr);

		if(status!= 200){
		client.stop();
		return status;
		}
		}

		if(NULL != strstr_P(line,PSTR("X-SESSION_ID"))){//Is this the line containing "X-SESSION_ID"?
		char *ptr;
		ptr = strtok(line, " ");
		ptr = strtok(NULL, " ");

		strncpy(sessionId_,ptr,sizeof(sessionId_));
		Serial.print(F("Received session_id: "));
		Serial.println(ptr);						

		client.stop();
		return status;
		}
		memset(line,0,sizeof(line));
		}

		}else{//Nothing available (yet).
		//Serial.println(F("No bytes available!"));
		//delay(100);
		}
		}*/
	}else{
		Serial.println(F("client.connect() failed!"));
		client.stop();
		return -1;
	}

	client.stop();
	return -1;
}

long CommonSense::createSensor(char *displayName, char *deviceType, char *name){
	Serial.println(F("createSensor()..."));

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){ 
		char line[256] = {'\0'};
		char chrIn[2] = {'\0'};
		int status = 1;

		sprintf_P(line,PSTR("{\"sensor\":{\"display_name\":\"%s\",\"device_type\":\"%s\",\"data_type\":\"json\",\"name\":\"%s\",\"data_structure\":\"{\\\"value\\\":\\\"Integer\\\"}\"}}"),displayName,deviceType,name);

		writeToClient(F("POST /sensors.json HTTP/1.1\r\n"), client);
		writeHeadersToClient(client,strlen(line),true);
		writeToClient(line, client);

		memset(line,0,sizeof(line));

		return *(static_cast<long *>(parseResponse(1, &status, client)));

		//clear the buffer just to be sure it's empty when we start writing to it again
		//delay(300);
		/*while(client.connected()){//While we are still connected...
		if(client.available()){//Are there bytes available to read?
		chrIn[0] = client.read();
		if('\r' != chrIn[0] || '\n' == chrIn[0])
		strncat(line, chrIn, 1);
		if('\r' == chrIn[0]){//We're done reading a line...
		if(NULL != strstr_P(line,PSTR("HTTP/1.1"))){ //Does the line we just read contain "HTTP/1.1"?
		char *ptr1 = NULL;
		ptr1 = strtok(line, " ");
		ptr1 = strtok(NULL, " ");

		Serial.print(F("Received status: "));
		Serial.println(ptr1);
		status = atoi(ptr1);

		if(status!= 201){
		client.stop();
		return status;
		}
		}

		if(NULL != strstr_P(line,PSTR("Location:"))){//Is this the line containing "Location"?
		char *ptr2;
		ptr2 = strtok(line, " ");
		ptr2 = strtok(NULL, "/");
		ptr2 = strtok(NULL, "/");
		ptr2 = strtok(NULL, "/");
		ptr2 = strtok(NULL, "/");

		Serial.print(F("Received sensor_id: "));
		Serial.println(ptr2);
		client.stop();
		return (atol(ptr2));
		}
		memset(line,0,sizeof(line));
		}

		}else{//Nothing available (yet).

		}
		}*/

	}else{
		Serial.println(F("client.connect() failed!"));
		client.stop();
		return 1;
	}

	client.stop();
	return 1;
}

int CommonSense::deleteSensor(long sensorId){
	Serial.println(F("deleteSensor()..."));

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){
		char line[256] = {'\0'}; //string buffer
		char chrIn[2] = {'\0'};
		int status = 1;

		sprintf_P(line,PSTR("DELETE /sensors/%ld HTTP/1.1\r\n"),sensorId);
		writeToClient(line, client);
		writeHeadersToClient(client,0,true);

		memset(line,0,sizeof(line));

		status = *static_cast<int *>(parseResponse(2, &status, client));
		return status;

		/*while(client.connected()){//While we are still connected...
		if(client.available()){//Are there bytes available to read?
		chrIn[0] = client.read();
		if('\r' != chrIn[0] || '\n' == chrIn[0])
		strncat(line, chrIn, 1);
		if('\r' == chrIn[0]){//We're done reading a line...
		if(NULL != strstr_P(line,PSTR("HTTP/1.1"))){ //Does the line we just read contain "HTTP/1.1"?
		char *ptr;
		ptr = strtok(line, " ");
		ptr = strtok(NULL, " ");

		Serial.print(F("Received status: "));
		Serial.println(ptr);
		status = atoi(ptr);

		client.stop();
		return status;
		}
		memset(line,0,sizeof(line));
		}

		}else{//Nothing available (yet).
		//Serial.println(F("No bytes available!"));
		//delay(100);
		}
		}*/
	}else{
		Serial.println(F("client.connect() failed!"));
		client.stop();
		return 1;
	}

	client.stop();
	return 1;
}

int CommonSense::uploadData(const long sensorId, const int value, unsigned long time){
	Serial.println(F("uploadData()..."));

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){
		char line[256] = {'\0'}; //buffer for the request and response;
		char chrIn[2] = {'\0'};
		int status = 1;

		sprintf_P(line,PSTR("POST /sensors/%ld/data.json HTTP/1.1\r\n"),sensorId);
		writeToClient(line, client);
		memset(line,0,sizeof(line));
		sprintf_P(line, PSTR("{\"data\":[{\"value\":%d,\"date\":%lu}]}"),value, time);
		writeHeadersToClient(client,strlen(line),true);
		writeToClient(line, client);

		memset(line,0,sizeof(line));

		parseResponse(3, &status, client);
		return status;

		/*while(client.connected()){
		if(client.available()){
		chrIn[0] = client.read();
		if('\r' != chrIn[0] || '\n' == chrIn[0])
		strncat(line, chrIn, 1);
		if('\r' == chrIn[0]){//We're done reading a line...
		if(NULL != strstr_P(line,PSTR("HTTP/1.1"))){ //Does the line we just read contain "HTTP/1.1"?
		char *ptr1 = NULL;
		ptr1 = strtok(line, " ");
		ptr1 = strtok(NULL, " ");

		status = atoi(ptr1);
		Serial.print(F("Received status: "));
		Serial.println(ptr1);
		client.stop();
		return status;
		}
		memset(line,0,sizeof(line));
		}
		}else{
		//delay(100);
		}
		}*/
	}else{
		Serial.println(F("client.connect() failed!"));
		client.stop();
		return 1;
	}

	client.stop();
	return 1;
}

int CommonSense::getLastData(const long sensorId){
	Serial.println(F("getLastData()..."));

	int status = -0;
	int data = -0;
	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){
		char line[256] = {'\0'}; //string buffer
		char chrIn[2] = {'\0'};

		sprintf_P(line,PSTR("GET /sensors/%ld/data?last=true HTTP/1.1\r\n"),sensorId);
		writeToClient(line, client);
		writeHeadersToClient(client,0,true);

		memset(line,0,sizeof(line));

		data = *static_cast<int *>(parseResponse(4, &status, client));
		return data;

		/*while(client.connected()){//While we are still connected...
		if(client.available()){//Are there bytes available to read?
		//Serial.print(chrIn[0] = client.read());
		chrIn[0] = client.read();
		if('\r' != chrIn[0] || '\n' == chrIn[0])
		strncat(line, chrIn, 1);
		if('\n' == chrIn[0]){//We're done reading a line...
		if(NULL != strstr_P(line,PSTR("HTTP/1.1"))){ //Does the line we just read contain "HTTP/1.1"?
		char *ptr;
		ptr = strtok(line, " ");
		ptr = strtok(NULL, " ");

		Serial.print(F("Received status: "));
		Serial.println(ptr);
		status = atoi(ptr);
		if(200!= status)
		return status;
		}
		if(NULL != strstr_P(line, PSTR("value"))){ //\"value\":
		char *t;
		char *ptr;

		t = strstr(line, "value");
		ptr = strtok(t, "\"");
		ptr = strtok(NULL, "\"");
		ptr = strtok(NULL, "\"");
		data = atoi(ptr);

		Serial.print("Received data value: ");
		Serial.println(data);
		client.stop();
		return data;
		}
		memset(line,0,sizeof(line));
		}

		}else{//Nothing available (yet).
		//Serial.println(F("No bytes available!"));
		//delay(100);
		}
		}*/
	}else{
		Serial.println(F("client.connect() failed!"));
		client.stop();
		return -0;
	}

	client.stop();
	return 999;
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

int CommonSense::writeHeadersToClient(EthernetClient &client, int length, bool sessionRequired){
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

void* CommonSense::parseResponse(int request, int* status, EthernetClient &client){
	/*
	request codes: 
	0 - login
	1 - create sensor
	2 - delete sensor
	3 - upload data
	4 - get latest data (int)
	*/
	const char *str[] = {"X-SESSION_ID","Location:","value"};
	int err = -1;
	char line[256] = {'\0'};
	char input[2] = {'\0'};

	while(client.connected()){//Do something while we are connected
		if(client.available()){//Do we have a byte? if so...

			input[0] = client.read();
			if('\r' != input[0] || '\n' == input[0])
				strncat(line, input, 1);
			if('\r' == input[0]){//We're done reading a line...
				if(NULL != strstr_P(line, PSTR("HTTP/1.1"))){ //Does the line we just read contain "HTTP/1.1"?
					char *ptr;
					ptr = strtok(line, " ");
					ptr = strtok(NULL, " ");

					//printf("parseResponse:Received status: %s\n", ptr);
					Serial.print(F("parseResponse:Received status: "));
					Serial.println(ptr);

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
						Serial.print(F("parseResponse:Received session_id: "));
						Serial.println(data);
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

						long data = atol(ptr);
						Serial.print(F("parseResponse:Received sensor_id: "));
						Serial.println(data);
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

						int data = atoi(ptr);
						Serial.print(F("parseResponse:Received value: "));
						Serial.println(data);
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

unsigned long CommonSense::time(){
	return NTPTime + (millis()/1000) - timeSinceStart;
}

int CommonSense::setupTime(){
	Udp.begin(8888u);

	NTPTime = getNtpTime();
	timeSinceStart = (millis() / 1000);
	Udp.stop();
	return 0;
}

unsigned long CommonSense::getNtpTime() //from UdpNtpClient example
{
	byte packetBuffer[NTP_PACKET_SIZE];
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	sendNTPpacket(timeServer);

	delay(1000);

	if (Udp.parsePacket()) {
		Udp.read(packetBuffer, NTP_PACKET_SIZE);

		unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
		unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

		unsigned long secsSince1900 = highWord << 16 | lowWord;

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
