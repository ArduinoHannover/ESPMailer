#include "ESPMailer.h"

ESPMailer::ESPMailer() {
}

ESPMailer::~ESPMailer() {
	free(_from);
	free(_fromName);
	free(_to);
	free(_toNames);
	free(_cc);
	free(_ccNames);
	free(_bcc);
	free(_bccNames);
	//free();
	//free();
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

char* ESPMailer::append(char*& dest, const char* src) {
	if (dest == NULL) return NULL;
	if (src == NULL) return dest;
	size_t dlen = strlen(dest);
	size_t slen = strlen(src);
	dest = (char*)realloc(dest, dlen+slen+2);
	dest[dlen] = '\n';
	memcpy(dest+dlen+1, src, slen+1);
	return dest;
}

void ESPMailer::setDebugLevel(uint8_t level) {
	do_debug = level;
}

boolean ESPMailer::sendCMD(const char* cmd, uint16_t code) {
	if (!_smtp.connected()) return false;
	if (cmd != NULL) {
		_smtp.println(cmd);
		if (do_debug > 0 && do_debug & 1) {
			Serial.write('>');
			Serial.println(cmd);
		}
	}
	uint32_t timeout = millis()+Timeout;
	while (_smtp.available() < 4 && millis() < timeout) delay(1);
	last_reply = _smtp.readStringUntil('\n');
	if (do_debug > 0 && do_debug & 2) {
		Serial.write('<');
		Serial.println(last_reply);
	}
	if (last_reply.indexOf(String(code)) == 0) return true;
	if (do_debug < 0) return false;
	Serial.println("Error while processing following command:");
	Serial.write('>');
	Serial.println(cmd);
	Serial.println(last_reply);
	return false;
}

void ESPMailer::setFrom(const char* from) {
	setFrom(from, NULL);
}
void ESPMailer::setFrom(const char* from, const char* fromName) {
	_from = (char*) malloc(strlen(from)+1);
	strcpy(_from, from);
	if (fromName != NULL) {
		_fromName = (char*) malloc(strlen(fromName)+1);
		strcpy(_fromName, fromName);
	}
}

void ESPMailer::addAddress(const char* to) {
	addAddress(to, NULL);
}
void ESPMailer::addAddress(const char* to, const char* name) {
	if (_to == NULL) {
		_to = (char*)malloc(strlen(to)+1);
		strcpy(_to, to);
	} else {
		append(_to, to);
	}
	if (_toNames == NULL) {
		_toNames = (char*)malloc(name==NULL?2:(strlen(name)+1));
		strcpy(_toNames, name==NULL?"\r":name);
	} else {
		append(_toNames, name==NULL?"\r":name);
	}
}

void ESPMailer::addCC(const char* cc) {
	addCC(cc, NULL);
}
void ESPMailer::addCC(const char* cc, const char* name) {
	if (_cc == NULL) {
		_cc = (char*)malloc(strlen(cc)+1);
		strcpy(_cc, cc);
	} else {
		append(_cc, cc);
	}
	if (_ccNames == NULL) {
		_ccNames = (char*)malloc(name==NULL?2:(strlen(name)+1));
		strcpy(_ccNames, name==NULL?"\r":name);
	} else {
		append(_ccNames, name==NULL?"\r":name);
	}
}

void ESPMailer::addBCC(const char* bcc) {
	addBCC(bcc, NULL);
}
void ESPMailer::addBCC(const char* bcc, const char* name) {
	if (_bcc == NULL) {
		_bcc = (char*)malloc(strlen(bcc)+1);
		strcpy(_bcc, bcc);
	} else {
		append(_bcc, bcc);
	}
	if (_bccNames == NULL) {
		_bccNames = (char*)malloc(name==NULL?2:(strlen(name)+1));
		strcpy(_bccNames, name==NULL?"\r":name);
	} else {
		append(_bccNames, name==NULL?"\r":name);
	}
}

void ESPMailer::setTimezone(float timezone) {
	_timezone = timezone;
}

boolean ESPMailer::isHTML() {
	return isHTML(html);
}
boolean ESPMailer::isHTML(boolean h) {
	return (html = h);
}

boolean ESPMailer::SMTPTo(char* arr) {
	char* buf = (char*)malloc(100);
	char* tok = strtok(arr, _delim);
	while (tok != NULL) {
		Serial.println(tok);
		sprintf(buf, "RCPT TO:<%s>", tok);
		Serial.println(buf);
		if (!sendCMD(buf, 250)) return false;
		tok = strtok(NULL, _delim);
	}
	free(buf);
}
boolean ESPMailer::HeaderTo(const char* type, char* arr, char* nameArr) {
	char* tok = strtok(arr, _delim);
	char* nameTok = strtok(nameArr, _delim);
	while (tok != NULL) {
		if (strcmp(nameTok, "\r"))
			_smtp.printf("%s: <%s>\r\n", type, tok);
		else
			_smtp.printf("%s: \"%s\" <%s>\r\n", type, nameTok, tok);
		tok = strtok(NULL, _delim);
		nameTok = strtok(NULL, _delim);
	}
}

boolean ESPMailer::send() {
	if (TLS) {
		_smtp = WiFiClientSecure();
	}

	NTP ntp = NTP();
	ntp.begin();
	
	char* buf = (char*)malloc(100);
	if (!_smtp.connect(Host.c_str(), Port)) {
		if (do_debug >= 0) {
			Serial.println("No connection to server!");
			Serial.println(Host);
			Serial.println(Port);
		}
		return false;
	}
	if (!sendCMD(NULL,220)) return false;
	
	sprintf(buf, "HELO %s", Host.c_str());
	if (!sendCMD(buf,250)) return false;
	
	//Does not work - for now.
	if (TLS) {
		if (sendCMD("STARTTLS",220)) {
			//Handshake needed here.
			if (!sendCMD(buf,250)) {
				return false; //resend HELO
			}
		} else {
			return false;
		}
	}
	
	if (SMTPAuth) {
		switch (AuthType) {
			case PLAIN: {
				if (!sendCMD("AUTH PLAIN",334)) return false;
				char plainAuth[Username.length()+Password.length()+3];
				plainAuth[0] = '\0';
				strcpy(&plainAuth[1], Username.c_str());
				strcpy(&plainAuth[2+Username.length()], Password.c_str());
				const char* pA = (const char*)&plainAuth;
				base64_encode(buf, pA, Username.length()+Password.length()+2);
				if (!sendCMD(buf,235)) return false;
				break;
			}
			case LOGIN: {
				if (!sendCMD("AUTH LOGIN",334)) return false;
				base64_encode(buf, Username.c_str(), Username.length());
				if (!sendCMD(buf,334)) return false;
				base64_encode(buf, Password.c_str(), Password.length());
				if (!sendCMD(buf,235)) return false;
				break;
			}
			case XOAUTH2:
			case NTLM:
			case CRAM_MD5: {
				if (do_debug >= 0) {
					Serial.println("AUTH Method currently not supported");
				}
				return false;
				/*
				//CRAM:
				//implement base64_decode and hmac
				if (!sendCMD("AUTH CRAM-MD5",334)) return false;
				String challenge = base64_decode(last_reply.substring(4));
				String md5resp = Username+" "+hmac($challenge, Password);
				if (!sendCMD(md5resp.c_str(),235)) return false;
				*/
			}
		}
	}
	
	sprintf(buf, "MAIL FROM:<%s>", _from);
	if (!sendCMD(buf,250)) return false;
	
	if (!SMTPTo(_to)) return false;
	if (!SMTPTo(_cc)) return false;
	if (!SMTPTo(_bcc)) return false;
	
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
	
	_smtp.printf("Message-Id: <%ld%ld@%s>\r\n", ESP.getChipId(),ESP.getCycleCount(), Host.c_str());
	_smtp.println("X-Mailer: ESPMailer v2.0 (http://github.com/ArduinoHannover/ESPMailer)");
	_smtp.printf("Subject: %s\r\n", Subject.c_str());
	if (_fromName != NULL)
		_smtp.printf("From: \"%s\" <%s>\r\n", _fromName, _from);
	else
		_smtp.printf("From: <%s>\r\n", _from);
	
	
	if (_to != NULL)
		HeaderTo("To", _to, _toNames);
	if (_cc != NULL)
		HeaderTo("CC", _cc, _ccNames);
	if (_bcc != NULL)
		HeaderTo("BCC", _bcc, _bccNames);
	
	if (AltBody.length() && html) {
		_smtp.println("MIME-Version: 1.0");
		_smtp.println("Content-Type: multipart/mixed; boundary=\"=ESP_MAIL_PART_MFASLAGAD\"");
		_smtp.println("This is a multipart message in MIME format.");
		_smtp.println("\r\n--=ESP_MAIL_PART_MFASLAGAD");
		_smtp.println("Content-Type: text/plain; charset=iso-8859-1");
		_smtp.println("Content-Disposition: inline");
		_smtp.println("Content-Transfer-Encoding: 8bit\r\n");
		_smtp.println(AltBody.c_str());
		_smtp.println("\r\n--=ESP_MAIL_PART_MFASLAGAD");
		_smtp.println("Content-Disposition: inline");
	}
	_smtp.printf("Content-Type: %s\r\n",html?"text/html":"text/plain");
	_smtp.println("Content-Transfer-Encoding: 8bit\r\n");
	_smtp.println(Body.c_str());
	if (AltBody.length() && html) _smtp.println("\r\n--=ESP_MAIL_PART_MFASLAGAD--");
	_smtp.println();
	if (!sendCMD(".",250)) return false;
	if (!sendCMD("QUIT",221)) return false;
}
