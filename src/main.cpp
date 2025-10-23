#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi Configuration
// Replace with your WiFi credentials
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

// MQTT Configuration
// Replace with your MQTT broker details
const char *serverHostname = "broker.hivemq.com";  // Free public broker for testing
const unsigned int serverPort = 1883;
const char *topic = "esp8266/led";  // MQTT topic for LED control

// Hardware Configuration
#define LED (2)  // Use onboard LED (GPIO2)

// Protocol Configuration
#define MAX_MSG_LEN (128)  // Maximum received message length

WiFiClient espClient;
PubSubClient client(espClient);

/**
 * Connect to WiFi network
 * Blocks until connection is established
 */
void connectWifi()
{
  delay(10);

  Serial.printf("\nConnecting to WiFi: %s\n", ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

/**
 * Connect to MQTT broker
 * Retries until connection is established
 */
void connectMQTT()
{
  while (!client.connected())
  {
    // Create a random client ID to avoid conflicts
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);

    Serial.printf("MQTT connecting as client %s...\n", clientId.c_str());

    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("MQTT connected successfully!");

      // Publish MAC address as an announcement
      String announcement = "Device online - MAC: " + WiFi.macAddress();
      client.publish(topic, announcement.c_str());

      // Subscribe to the control topic
      client.subscribe(topic);
      Serial.printf("Subscribed to topic: %s\n", topic);
    }
    else
    {
      Serial.printf("MQTT connection failed, state: %d, retrying in 2.5s...\n", client.state());
      delay(2500);
    }
  }
}

/**
 * Set LED state
 * @param state true for ON, false for OFF
 * Note: LED logic is inverted (LOW = ON, HIGH = OFF)
 */
void setLedState(boolean state)
{
  digitalWrite(LED, !state);  // Inverted logic for ESP8266 onboard LED
  Serial.printf("LED turned %s\n", state ? "ON" : "OFF");
}

/**
 * MQTT message callback
 * Called when a message is received on subscribed topics
 */
void callback(char *msgTopic, byte *msgPayload, unsigned int msgLength)
{
  // Copy payload to a null-terminated string
  static char message[MAX_MSG_LEN + 1];
  if (msgLength > MAX_MSG_LEN)
  {
    msgLength = MAX_MSG_LEN;
  }
  strncpy(message, (char *)msgPayload, msgLength);
  message[msgLength] = '\0';

  Serial.printf("Message received on topic '%s': %s\n", msgTopic, message);

  // Process commands (case-insensitive)
  String msg = String(message);
  msg.toLowerCase();

  if (msg == "on" || msg == "1")
  {
    setLedState(true);
    client.publish(topic, "LED: ON");
  }
  else if (msg == "off" || msg == "0")
  {
    setLedState(false);
    client.publish(topic, "LED: OFF");
  }
  else if (msg == "toggle")
  {
    static boolean ledState = false;
    ledState = !ledState;
    setLedState(ledState);
    client.publish(topic, ledState ? "LED: ON" : "LED: OFF");
  }
  else if (msg == "status")
  {
    String status = "Device: ESP8266, IP: " + WiFi.localIP().toString() +
                    ", MAC: " + WiFi.macAddress();
    client.publish(topic, status.c_str());
  }
  else
  {
    Serial.printf("Unknown command: %s\n", message);
    client.publish(topic, "Error: Unknown command");
  }
}

/**
 * Setup function - runs once at startup
 */
void setup()
{
  // Initialize LED pin as output and turn it off
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);  // HIGH = OFF (inverted logic)

  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("\n\n========================================");
  Serial.println("ESP8266 MQTT LED Controller");
  Serial.println("========================================");

  // Connect to WiFi (blocking call)
  connectWifi();

  // Configure MQTT client
  client.setServer(serverHostname, serverPort);
  client.setCallback(callback);

  Serial.println("Setup complete!");
}

/**
 * Main loop - runs continuously
 */
void loop()
{
  // Reconnect to MQTT if connection is lost
  if (!client.connected())
  {
    connectMQTT();
  }

  // Process incoming MQTT messages (ESSENTIAL!)
  client.loop();

  // Small delay to prevent overwhelming the CPU
  delay(500);
}
