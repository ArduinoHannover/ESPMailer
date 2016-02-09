/*
 * ESP Mailer - TestMail.ino
 * ESP Mailer is a PHPMailer like Library for your ESP.
 * Check out https://github.com/ArduinoHannover/ESPMailer
 * And for Pushover Notifications: https://github.com/ArduinoHannover/Pushover
 */


#include <ESP8266WiFi.h>
#include "ESPMailer.h"
#include <Time.h>
#include <NTP.h>

const char* ssid = "myNetwork";
const char* key = "12345678";

void setup() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, key);
  	Serial.begin(115200);
  	Serial.println();
  	while (WiFi.status() != WL_CONNECTED) {
  		Serial.write('.');
  		delay(200);
  	}
  	
	Serial.println("Connected to WiFi");
	ESPMailer* mail = new ESPMailer();
	/*
	 * -1 = muted
	 *  0 = only errors
	 *  1 = Client -> Server
	 *  2 = Server -> Client
	 *  3 = both
	 */
	mail->setDebugLevel(3);
	mail->Host = "smtp.example.org";
	mail->SMTPAuth = true;
	mail->AuthType = LOGIN;
	mail->Username = "mailer";
	mail->Password = "foobar";
	mail->setFrom("mail@example.org","ESP8266 Example Mailer");
	mail->setTimezone(1); //defaults to UTC
	mail->addAddress("test@example.org");
	mail->addBCC("bcc@example.org");
	mail->Subject = "Test!";
	mail->isHTML(true);
	mail->Body = "<html><body>Hello <strong>ESP</strong> user!</body></html>";
	mail->AltBody = "Hello ESP user!";
	if (mail->send())
		Serial.println("Mail sent sucessfully!");
}

void loop() {
}