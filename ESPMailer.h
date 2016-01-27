#include <ESP8266WiFi.h>
#include <Time.h>
#include <NTP.h>
#ifndef NTP_H
#error "You haven't installed the ESP-NTP Library. It can be downloaded from http://github.com/ArduinoHannover/NTP"
#endif

#ifndef MAIL_H
#define MAIL_H

#ifndef null
#define null NULL
#endif


class ESPMailer {
	private:
		const char* _b64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		const char* _delim = "\n";
		WiFiClient _smtp;
		char* _server;
		char* _username;
		char* _password;
		char* _from;
		char* _fromName;
		String _to;
		String _cc;
		String _bcc;
		String _message;
		String _subject;
		int _port;
		float _timezone;
		boolean sendCMD(const char*, uint16_t);
		void a3_to_a4(unsigned char*, unsigned char*);
		int base64_encode(char*, const char*, int);
		void init(const char*, const char*, const char*, int);
	public:
		ESPMailer(const char*, const char*, const char*);
		ESPMailer(const char*, const char*, const char*, int);
		void setFrom(const char*);
		void setFrom(const char*, const char*);
		void addTo(String);
		void addCC(String);
		void addBCC(String);
		void message(String);
		void subject(String);
		void setTimezone(float);
		boolean send();
};
#endif
