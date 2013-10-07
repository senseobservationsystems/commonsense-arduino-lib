#include "CommonSense.h"

CommonSense::CommonSense(const char *username, const char *password, const char *deviceName, const byte *MAC) : username_(username), password_(password), deviceName_(deviceName), MAC_(MAC){ 

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
		char line[128] = {'\0'}; //string buffer
		char chrIn[2] = {'\0'};
		int status = 1;

		//Serial.println(F("Connected!"));

		sprintf_P(line,PSTR("{\"username\":\"%s\",\"password\":\"%s\"}"),username_,password_);
		//Serial.println(line);

		writeToClient(F("POST /login.json HTTP/1.1\r\n"), client);
		writeToClient(F("Host: api.sense-os.nl\r\n"), client);
		writeToClient(F("X-TARGET-URI: http://api.sense-os.nl\r\n"), client);
		writeToClient(F("Content-Type: application/json; charset=UTF-8\r\n"), client);
		writeToClient(F("Content-Length: "), client);client.print(strlen(line));writeToClient(F("\r\n"), client);
		writeToClient(F("Accept: *\r\n"), client);
		writeToClient(F("Connection: close\r\n"), client);
		writeToClient(F("\r\n"), client);
		writeToClient(line, client);		

		memset(line,0,sizeof(line));
		//delay(100);
		while(client.connected()){//While we are still connected...
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
		}
	}else{
		Serial.println(F("client.connect() failed!"));
		client.stop();
		return 1;
	}

	client.stop();
	return 1;
}

long CommonSense::createSensor(char *displayName, char *deviceType, char *name){
	Serial.println(F("createSensor()..."));

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){ 
		char line[256] = {'\0'};
		char chrIn[2] = {'\0'};
		int status = 1;

		//Serial.println(F("Connected!"));

		sprintf_P(line,PSTR("{\"sensor\":{\"display_name\":\"%s\",\"device_type\":\"%s\",\"data_type\":\"json\",\"name\":\"%s\",\"data_structure\":\"{\\\"value\\\":\\\"Integer\\\"}\"}}"),displayName,deviceType,name);
		//Serial.println(line);

		writeToClient(F("POST /sensors.json HTTP/1.1\r\n"), client);
		writeToClient(F("Host: api.sense-os.nl\r\n"), client);
		writeToClient(F("X-TARGET-URI: http://api.sense-os.nl\r\n"), client);
		writeToClient(F("X-SESSION_ID: "), client);writeToClient(sessionId_, client);writeToClient(F("\r\n"), client);
		writeToClient(F("Content-Type: application/json; charset=UTF-8\r\n"), client);
		writeToClient(F("Content-Length: "), client);client.print(strlen(line));writeToClient(F("\r\n"), client);
		writeToClient(F("Accept: *\r\n"), client);
		writeToClient(F("Connection: close\r\n"), client);
		writeToClient(F("\r\n"), client);
		writeToClient(line, client);

		memset(line,0,sizeof(line)); //clear the buffer just to be sure it's empty when we start writing to it again
		//delay(300);
		while(client.connected()){//While we are still connected...
			if(client.available()){//Are there bytes available to read?
				//Serial.print(chrIn[0] = client.read());
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
				//Serial.println(F("No bytes available!"));
				//delay(100);
			}
		}

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
		char line[128] = {'\0'}; //buffer for the request and response;
		char chrIn[2] = {'\0'};
		int status = 1;

		//Serial.println(F("Connected!"));

		sprintf_P(line,PSTR("POST /sensors/%ld/data.json HTTP/1.1\r\n"),sensorId);
		//Serial.println(line);

		writeToClient(line, client);
		writeToClient(F("Host: api.sense-os.nl\r\n"), client);
		writeToClient(F("X-TARGET-URI: http://api.sense-os.nl\r\n"), client);
		writeToClient(F("X-SESSION_ID: "), client);writeToClient(sessionId_, client);writeToClient(F("\r\n"), client);
		memset(line,0,sizeof(line));
		sprintf_P(line, PSTR("{\"data\":[{\"value\":%d,\"date\":%lu}]}"),value, time);
		//Serial.println(line);
		writeToClient(F("Content-Type: application/json; charset=UTF-8\r\n"), client);
		writeToClient(F("Content-Length: "), client);client.print(strlen(line));writeToClient(F("\r\n"), client);
		writeToClient(F("Accept: *\r\n"), client);
		writeToClient(F("Connection: close\r\n"), client);
		writeToClient(F("\r\n"), client);
		writeToClient(line, client);

		memset(line,0,sizeof(line));
		//delay(300);
		while(client.connected()){
			if(client.available()){
				//Serial.print(chrIn[0] = client.read());
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
				//Serial.println(F("No bytes available!"));
				//delay(100);
			}
		}
	}else{
		Serial.println(F("client.connect() failed!"));
		client.stop();
		return 1;
	}

	client.stop();
	return 1;
}

int CommonSense::getLastData(const long sensorId){
	int data;


	return data;
}

int CommonSense::deleteSensor(long sensorId){
	Serial.println(F("deleteSensor()..."));

	EthernetClient client;
	client.connect(server, 80);

	if(client.connected()){
		char line[128] = {'\0'}; //string buffer
		char chrIn[2] = {'\0'};
		int status = 1;

		//Serial.println(F("Connected!"));

		sprintf_P(line,PSTR("DELETE /sensors/%ld HTTP/1.1\r\n"),sensorId);
		//Serial.println(line);

		writeToClient(line, client);
		writeToClient(F("Host: api.sense-os.nl\r\n"), client);
		writeToClient(F("X-TARGET-URI: http://api.sense-os.nl\r\n"), client);
		writeToClient(F("X-SESSION_ID: "), client);writeToClient(sessionId_, client);writeToClient(F("\r\n"), client);
		writeToClient(F("Accept: *\r\n"), client);
		writeToClient(F("Connection: close\r\n"), client);
		writeToClient(F("\r\n"), client);

		memset(line,0,sizeof(line));
		//delay(300);
		while(client.connected()){//While we are still connected...
			if(client.available()){//Are there bytes available to read?
				Serial.print(chrIn[0] = client.read());
				//chrIn[0] = client.read();
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
		}
	}else{
		Serial.println(F("client.connect() failed!"));
		client.stop();
		return 1;
	}

	client.stop();
	return 1;
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

unsigned long CommonSense::time(){
	return NTPTime + (millis()/1000) - timeSinceStart;
}

int setupTime(){
	Udp.begin(8888u);

	NTPTime = getNtpTime();
	timeSinceStart = (millis() / 1000);// Time.h function
	//Serial.print(F("Time has been set to: "));
	//Serial.println(now());
	Udp.stop();
	return 0;
}

unsigned long getNtpTime() //from UdpNtpClient example
{
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

void sendNTPpacket(IPAddress &address)// From Time example TimeNTP
	// send an NTP request to the time server at the given address
{
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
