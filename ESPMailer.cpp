#include "ESPMailer.h"

ESPMailer::ESPMailer(const char* server, const char* username, const char* password) {
	init(server, username, password, 25);
}

ESPMailer::ESPMailer(const char* server, const char* username, const char* password, int port) {
	init(server, username, password, port);
}

void ESPMailer::init(const char* server, const char* username, const char* password, int port) {
	_server = (char*)malloc(strlen(server));
	strcpy(_server, server);
	_username = (char*)malloc((strlen(username)+2)/3*4+1);
	base64_encode(_username, username, strlen(username));
	_password = (char*)malloc((strlen(password)+2)/3*4+1);
	base64_encode(_password, password, strlen(password));
	_port = port;
}

void ESPMailer::a3_to_a4(unsigned char * a4, unsigned char * a3) {
	a4[0] = (a3[0] & 0xfc) >> 2;
	a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
	a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
	a4[3] = (a3[2] & 0x3f);
}

int ESPMailer::base64_encode(char *output, const char *input, int inputLen) {
	int i = 0, j = 0;
	int encLen = 0;
	unsigned char a3[3];
	unsigned char a4[4];
	while(inputLen--) {
		a3[i++] = *(input++);
		if(i == 3) {
			a3_to_a4(a4, a3);
			for(i = 0; i < 4; i++) {
				output[encLen++] = _b64_alphabet[a4[i]];
			}
			i = 0;
		}
	}
	if(i) {
		for(j = i; j < 3; j++) {
			a3[j] = '\0';
		}
		a3_to_a4(a4, a3);
		for(j = 0; j < i + 1; j++) {
			output[encLen++] = _b64_alphabet[a4[j]];
		}
		while((i++ < 3)) {
			output[encLen++] = '=';
		}
	}
	output[encLen] = '\0';
	return encLen;
}

boolean ESPMailer::sendCMD(const char* cmd, uint16_t code) {
	if (!_smtp.connected()) return false;
	if (cmd != null)
		_smtp.println(cmd);
	String msg = _smtp.readStringUntil('\n');
	if (msg.indexOf(String(code)) == 0) return true;
	Serial.println("Error while processing following command:");
	Serial.write('>');
	Serial.println(cmd);
	Serial.println(msg);
}

void ESPMailer::setFrom(const char* from) {
	setFrom(from, "ESP8266");
}

void ESPMailer::setFrom(const char* from, const char* fromName) {
	_from = (char*) malloc(strlen(from));
	strcpy(_from, from);
	_fromName = (char*) malloc(strlen(fromName));
	strcpy(_fromName, fromName);
}

void ESPMailer::addTo(String to) {
	_to += to+"\n";
}

void ESPMailer::addCC(String cc) {
	_cc += cc+"\n";
}

void ESPMailer::addBCC(String bcc) {
	_bcc += bcc+"\n";
}

void ESPMailer::message(String message) {
	_message = message;
}

void ESPMailer::subject(String subject) {
	_subject = subject;
}

void ESPMailer::setTimezone(float timezone) {
	_timezone = timezone;
}

boolean ESPMailer::send() {
	NTP ntp = NTP();
	ntp.begin();
	char* buf = (char*)malloc(100);
	if (!_smtp.connect(_server, _port)) {
		Serial.println("No connection to server!");
		return false;
	}
	if (!sendCMD(null,220)) return false;
	strcpy(buf, "HELO ");
	strcat(buf, _server);
	if (!sendCMD(buf,250)) return false;
	if (!sendCMD("AUTH LOGIN",334)) return false;
	if (!sendCMD(_username,334)) return false;
	if (!sendCMD(_password,235)) return false;
	strcpy(buf, "MAIL FROM:<");
	strcat(buf, _from);
	strcat(buf, ">");
	if (!sendCMD(buf,250)) return false;
	String rcpt_s = _to+_cc+_bcc;
	char* rcpt = new char[rcpt_s.length()+1];
	strcpy(rcpt, rcpt_s.c_str());
	char* tok = strtok(rcpt, _delim);
	while (tok != null) {
		strcpy(buf, "RCPT TO:<");
		strcat(buf, tok);
		strcat(buf, ">");
		if (!sendCMD(buf,250)) return false;
		tok = strtok(null, _delim);
	}
	delete [] rcpt;

	
	if (!sendCMD("DATA",354)) return false;

	time_t tm = ntp.get(_timezone);
	//must be separated due to pointer overload
	_smtp.printf("Date: %s, ",
		dayShortStr(weekday(tm))
	);
	_smtp.printf("%02d %s %04d %02d:%02d:%02d %+03d%02d\r\n",
		day(tm),
		monthShortStr(month(tm)),
		year(tm),
		hour(tm),
		minute(tm),
		second(tm),
		(int)(_timezone),
		floor(_timezone) == _timezone ? 0 : 30
	);
	
	_smtp.printf("Message-Id: <%ld%ld@%s>\r\n", ESP.getChipId(),ESP.getCycleCount(), _server);
	_smtp.println("X-Mailer: ESPMailer (http://github.com/ArduinoHannover/ESPMailer)");
	_smtp.println("Content-Type: text/plain");
	_smtp.println("Content-Transfer-Encoding: 8bit");
	_smtp.printf("Subject: %s\r\n", _subject.c_str());
	_smtp.printf("From: \"%s\" <%s>\r\n", _fromName, _from);
	char* to_tmp = new char[_to.length()+1];
	strcpy(to_tmp, _to.c_str());
	tok = strtok(to_tmp, _delim);
	while (tok != null) {
		_smtp.printf("To: <%s>\r\n",tok);
		tok = strtok(null, _delim);
	}
	delete [] to_tmp;
	char* cc_tmp = new char[_cc.length()+1];
	strcpy(cc_tmp, _cc.c_str());
	tok = strtok(cc_tmp, _delim);
	while (tok != null) {
		_smtp.printf("CC: <%s>",tok);
		tok = strtok(null, _delim);
	}
	delete [] cc_tmp;
	char* bcc_tmp = new char[_bcc.length()+1];
	strcpy(bcc_tmp, _bcc.c_str());
	tok = strtok(bcc_tmp, _delim);
	while (tok != null) {
		_smtp.printf("BCC: %s\r\n",tok);
		tok = strtok(null, _delim);
	}
	delete [] bcc_tmp;
	_smtp.println();
	_smtp.println(_message);
	_smtp.println();
	if (!sendCMD(".",250)) return false;
	if (!sendCMD("QUIT",221)) return false;
}
