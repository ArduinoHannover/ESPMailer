# ESPMailer
SMTP Client for ESP8266

## Dependencies:
[NTP-Class](https://github.com/ArduinoHannover/NTP) (otherwise you won't know the date, without Date: the message may be lost in spam)

[Time-Class](https://github.com/PaulStoffregen/Time) (parsing of UNIX-Timestamp made easy)


##Usage

`ESPMailer mail = ESPMailer(host, user, password, [port])` Create a new mail, port defaults to 25

`mail.setFrom(address, [name])` Set sender, optional with name, otherwise it will be set to ESP8266

`mail.addTo(address)` Add a new recipient, could be `addCC` or `addBCC` too.

`mail.message(plaintext)` Set the message, currently only plaintext available. Attribute `_message` may get public as `Body` to get closer to PHPMailer.

`mail.subject(subject)` Same transformation may apply here.

`mail.setTimezone(UTC)` Difference from UTC in hours, xx.5 accepted for XX:30 time difference. If not called, UTC is used in Timestamp.

`mail.send()` Returns true if sent successfully.

### TODO

* Add more and alter functions to match PHPMailer.

* Parse message to detect single dot in a line -> injection currently possible.
