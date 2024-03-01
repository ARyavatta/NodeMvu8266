#include <Arduino.h>
#include <ESP8266WiFi.h>

#define LED_BUILTIN 2 // Most ESP8266 boards use GPIO2 for the built-in LED, but this can vary. Adjust if necessary.

const char *ssid = "RoyNiketan_2.4";
const char *password = "Assassin#0";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200);
  delay(10);

  // Start the connection attempt to the WiFi network
  Serial.println();
  Serial.print("Attempting to connect to WiFi network: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
}

void loop() {
  static bool tryingToConnect = true;

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off (LED logic might be inverted depending on the board)
    delay(1000); // Slow blink - Not connected
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on
    delay(1000); // Slow blink - Not connected

    if (tryingToConnect) {
      Serial.println("Trying to connect to WiFi...");
    }
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
    delay(200); // Fast blink - Connected
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on
    delay(200); // Fast blink - Connected

    if (tryingToConnect) {
      Serial.println("WiFi connected successfully!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      tryingToConnect = false; // Prevent repeating the connection message
    }
  }
}