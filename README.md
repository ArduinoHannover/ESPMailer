# ESPMailer
SMTP Client for ESP8266

## Dependencies:
[NTP-Class](https://github.com/ArduinoHannover/NTP) (otherwise you won't know the date, without Date: the message may be lost in spam)

[Time-Class](https://github.com/PaulStoffregen/Time) (parsing of UNIX-Timestamp made easy)


##Usage

`ESPMailer* mail = new ESPMailer()`: Create a new mail.

`mail->Host`: Address of your server.

`mail->Port`: Defaults to 25, change if you need.

`mail->SMTPAuth`: Set to true to enable authentification, then specify `->Username` and `->Password`

`mail->AuthType`: Defaults to `PLAIN`, can set to `LOGIN`; Other methods may be implemented.

`mail->setFrom(address, [name])`: Set sender, optional with name.

`mail->addAddress(address, [name])`: Add a new recipient, could be `addCC` or `addBCC` too.

`mail->Body`: The main message, if in HTML, you can use `AltBody` to define plaintext body.

`mail->isHTML([true/false])`: Returns, if message is set to html, may set the type, if you call with boolean.

`mail->Subject`: What could this be?

`mail->setTimezone(hours)`: Difference from UTC in hours, xx.5 accepted for XX:30 time difference (e.g. Venezuela: -4.5 will be -0430). If not called, UTC is used in Timestamp.

`mail->setDebugLevel(level)`: `-1` will be quiet, `0` just print errors (default), `1` messages from Client to Server, `2` Server answers and `3` will output everything

`mail->send()` Returns true if sent successfully.

### TODO

* Add more functions to match PHPMailer.

* SMTPS Support (#2)
