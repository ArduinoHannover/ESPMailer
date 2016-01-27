#include <ESP8266WiFi.h>
#include "ESPMailer.h"
#include <Time.h>

const char* smtp_server = "smtp.example.org";
const char* smtp_username = "mail@example.org";
const char* smtp_password = "12345678";
const char* ssid = "myNetwork";
const char* key = "12345678";

void setup() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, key);
  	Serial.begin(115200);
  	while (WiFi.status() != WL_CONNECTED) {
  		Serial.write('.');
  		delay(200);
  	}
  	
	Serial.println("Connected to WiFi");
	ESPMailer mail = ESPMailer(smtp_server, smtp_username, smtp_password);
	mail.setFrom("mail@example.org","ESP8266 Example Mailer");
	mail.setTimezone(1); //defaults to UTC
	mail.addTo("test@example.org");
	mail.addBCC("bcc@example.org");
	mail.subject("Test!");
	mail.message("Hello ESP user!");
	if (mail.send())
		Serial.println("Mail sent sucessfully!");
}

void loop() {
}
