#include <Arduino.h>

#define LED_BUILTIN 2 // Most ESP8266 boards use GPIO2 for the built-in LED, but this can vary. Adjust if necessary.

unsigned int counter = 0; // Initialize counter to 0

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200); // Initialize serial communication at 115200 bits per second
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note: Depending on the board, LOW might turn it off)
  delay(1000); // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off (Note: Depending on the board, HIGH might turn it on)
  delay(1000); // Wait for a second

  counter++; // Increment the counter
  Serial.println("light No: " + counter); // Print the current counter value to the Serial Monitor
}