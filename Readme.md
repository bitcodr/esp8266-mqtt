# MQTT subscription in ESP8266 board

## Configuration:
> const char *ssid = "your wifi name";

> const char *password = "your wifi password";

> const char *serverHostname = "mqtt host name or use broker.hivemq.com for development";

> const char *topic = "mqtt topic name";

Compile the code to the board by Vscode PlatformIO or Arduino IDE, Publish a message like `on` to the MQTT broker you'll see the board built-in LED turns on and send `off` for turning off built-in LED
