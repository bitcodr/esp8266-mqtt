# ESP8266 MQTT LED Controller

A simple IoT project that controls an ESP8266's onboard LED via MQTT messages. This project demonstrates how to connect an ESP8266 to WiFi, communicate with an MQTT broker, and control hardware based on received messages.

## Features

- WiFi connectivity with automatic reconnection
- MQTT client with automatic broker reconnection
- LED control via MQTT messages
- Multiple command support (on, off, toggle, status)
- Device status reporting (IP address, MAC address)
- Serial debug output for monitoring

## Hardware Requirements

- ESP8266 board (ESP-12E or compatible)
- USB cable for programming and power
- Built-in LED (GPIO2)

## Software Requirements

- [PlatformIO](https://platformio.org/) or [Arduino IDE](https://www.arduino.cc/en/software)
- PubSubClient library (automatically installed by PlatformIO)

## Installation

### Using PlatformIO (Recommended)

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/esp8266-mqtt.git
   cd esp8266-mqtt
   ```

2. Open the project in PlatformIO (VS Code with PlatformIO extension)

3. Install dependencies:
   ```bash
   pio lib install
   ```

### Using Arduino IDE

1. Install the Arduino IDE
2. Install the ESP8266 board support:
   - File → Preferences → Additional Board Manager URLs
   - Add: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
3. Install PubSubClient library:
   - Sketch → Include Library → Manage Libraries
   - Search for "PubSubClient" and install

## Configuration

Before uploading the code, you need to configure your WiFi and MQTT settings in `src/main.cpp`:

```cpp
// WiFi Configuration
const char *ssid = "YOUR_WIFI_SSID";           // Replace with your WiFi name
const char *password = "YOUR_WIFI_PASSWORD";   // Replace with your WiFi password

// MQTT Configuration
const char *serverHostname = "broker.hivemq.com";  // Replace with your MQTT broker
const unsigned int serverPort = 1883;               // MQTT port (default: 1883)
const char *topic = "esp8266/led";                  // MQTT topic for LED control
```

### Free MQTT Brokers for Testing

- `broker.hivemq.com` (public, no authentication)
- `test.mosquitto.org` (public, no authentication)
- `broker.emqx.io` (public, no authentication)

For production use, consider setting up your own MQTT broker using [Mosquitto](https://mosquitto.org/) or [HiveMQ](https://www.hivemq.com/).

## Building and Uploading

### PlatformIO

1. Connect your ESP8266 board via USB
2. Build and upload:
   ```bash
   pio run --target upload
   ```
3. Monitor serial output:
   ```bash
   pio device monitor
   ```

### Arduino IDE

1. Select your board: Tools → Board → ESP8266 Boards → Generic ESP8266 Module (or your specific board)
2. Select the correct port: Tools → Port
3. Click Upload button
4. Open Serial Monitor (115200 baud)

## Usage

### MQTT Commands

Once the device is connected to WiFi and the MQTT broker, you can send the following commands to the configured topic:

| Command | Description | Example |
|---------|-------------|---------|
| `on` or `1` | Turn LED on | `mosquitto_pub -h broker.hivemq.com -t esp8266/led -m "on"` |
| `off` or `0` | Turn LED off | `mosquitto_pub -h broker.hivemq.com -t esp8266/led -m "off"` |
| `toggle` | Toggle LED state | `mosquitto_pub -h broker.hivemq.com -t esp8266/led -m "toggle"` |
| `status` | Request device status | `mosquitto_pub -h broker.hivemq.com -t esp8266/led -m "status"` |

### Testing with Mosquitto Client

Install mosquitto-clients:
```bash
# Ubuntu/Debian
sudo apt-get install mosquitto-clients

# macOS
brew install mosquitto
```

Subscribe to the topic to see responses:
```bash
mosquitto_sub -h broker.hivemq.com -t esp8266/led
```

Publish commands (in another terminal):
```bash
# Turn LED on
mosquitto_pub -h broker.hivemq.com -t esp8266/led -m "on"

# Turn LED off
mosquitto_pub -h broker.hivemq.com -t esp8266/led -m "off"

# Toggle LED
mosquitto_pub -h broker.hivemq.com -t esp8266/led -m "toggle"

# Get device status
mosquitto_pub -h broker.hivemq.com -t esp8266/led -m "status"
```

### Testing with MQTT Explorer

[MQTT Explorer](http://mqtt-explorer.com/) is a great GUI tool for testing MQTT communications:

1. Download and install MQTT Explorer
2. Connect to your broker (e.g., broker.hivemq.com)
3. Navigate to your topic (e.g., esp8266/led)
4. Publish messages and see responses in real-time

## Serial Monitor Output

The device provides detailed debug information via serial output at 9600 baud:

```
========================================
ESP8266 MQTT LED Controller
========================================

Connecting to WiFi: YourWiFiName
.....
WiFi connected! IP address: 192.168.1.100
MAC address: AA:BB:CC:DD:EE:FF
MQTT connecting as client ESP8266-A3F2...
MQTT connected successfully!
Subscribed to topic: esp8266/led
Setup complete!
Message received on topic 'esp8266/led': on
LED turned ON
```

## Troubleshooting

### WiFi Connection Issues

- Verify SSID and password are correct
- Check that your WiFi network is 2.4GHz (ESP8266 doesn't support 5GHz)
- Ensure the ESP8266 is within range of your router

### MQTT Connection Issues

- Verify the MQTT broker hostname is correct
- Check if the broker requires authentication (update code accordingly)
- Test broker connectivity from your computer first
- Some public brokers may be temporarily unavailable - try another one

### LED Not Responding

- Verify the correct GPIO pin is used (GPIO2 for most ESP8266 boards)
- Check serial monitor to confirm messages are being received
- Note that LED logic is inverted (LOW = ON, HIGH = OFF)

### Serial Monitor Shows Garbage

- Ensure baud rate is set to 9600
- Some boards may show boot messages at different baud rates - this is normal

## Project Structure

```
esp8266-mqtt/
├── src/
│   └── main.cpp          # Main application code
├── include/              # Header files (if any)
├── lib/                  # Custom libraries (if any)
├── test/                 # Test files (if any)
├── platformio.ini        # PlatformIO configuration
├── Readme.md            # This file
└── .gitignore           # Git ignore rules
```

## Code Overview

### Main Components

- **connectWifi()**: Establishes WiFi connection
- **connectMQTT()**: Connects to MQTT broker and subscribes to topic
- **callback()**: Handles incoming MQTT messages and controls LED
- **setLedState()**: Controls the LED state
- **setup()**: Initializes hardware and connections
- **loop()**: Maintains MQTT connection and processes messages

### Supported Commands

The callback function supports multiple commands with case-insensitive matching:
- LED control: `on`, `off`, `1`, `0`, `toggle`
- Status reporting: `status`
- Error handling for unknown commands

## Extending the Project

### Add More Sensors/Actuators

1. Define new GPIO pins in the configuration section
2. Initialize them in `setup()`
3. Add command handling in `callback()`

Example - Add a temperature sensor:
```cpp
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// In callback function:
else if (msg == "temperature")
{
  float temp = dht.readTemperature();
  String response = "Temperature: " + String(temp) + "C";
  client.publish(topic, response.c_str());
}
```

### Add Authentication

If your MQTT broker requires authentication, modify the `connectMQTT()` function:

```cpp
const char* mqtt_user = "your_username";
const char* mqtt_password = "your_password";

// In connectMQTT():
if (client.connect(clientId.c_str(), mqtt_user, mqtt_password))
{
  // ... rest of the code
}
```

### Use Separate Topics

For better organization, use separate topics for commands and status:

```cpp
const char *command_topic = "esp8266/led/command";
const char *status_topic = "esp8266/led/status";

// Subscribe to command topic
client.subscribe(command_topic);

// Publish status to status topic
client.publish(status_topic, "LED: ON");
```

## Security Considerations

- **Never commit WiFi passwords** to public repositories
- Use environment variables or separate config files for credentials
- Consider using MQTT over TLS (port 8883) for production
- Implement authentication for your MQTT broker
- Use unique, strong passwords
- Consider using certificate-based authentication for production deployments

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is open source and available under the MIT License.

## Resources

- [ESP8266 Arduino Core Documentation](https://arduino-esp8266.readthedocs.io/)
- [PubSubClient Library](https://pubsubclient.knolleary.net/)
- [MQTT Protocol](https://mqtt.org/)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [HiveMQ MQTT Essentials](https://www.hivemq.com/mqtt-essentials/)

## Author

Amir Roshanaei - [bitcodr](https://github.com/bitcodr)

## Acknowledgments

- Nick O'Leary for the PubSubClient library
- ESP8266 Arduino core team
- MQTT community
