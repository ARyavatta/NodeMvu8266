#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> // For HTTPS
#include <ArduinoJson.h>

const char* ssid = "RoyNiketan_2.4";
const char* password = "Assassin#0";
const char* serverUrl = "https://atipre.azurewebsites.net/api/Esp8266";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastTime > 10000) { // Every 10 seconds
    lastTime = currentTime;

    // Create a JSON object
    StaticJsonDocument<200> jsonDocument;
    jsonDocument["id"] = 0;
    jsonDocument["deviceName"] = "ESP8266_Device";
    jsonDocument["currentIP"] = WiFi.localIP().toString();

    // Serialize JSON to a string
    String payload;
    serializeJson(jsonDocument, payload);

    if (WiFi.status() == WL_CONNECTED) { // Check if we're still connected to WiFi
      WiFiClientSecure client;
      HTTPClient http;

      client.setInsecure(); // Ignore SSL certificate validation (if needed)

      http.begin(client, serverUrl); // Start connection and send HTTP header
      http.addHeader("Content-Type", "application/json"); // Specify content-type header

      int httpResponseCode = http.POST(payload); // Send the actual POST request

      if (httpResponseCode > 0) { // Check the returning code
        String response = http.getString(); // Get the request response payload
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.println("Response: " + response);
      }
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end(); // Close connection
    }
  }
  delay(1000); // Small delay to prevent constant loop iteration if there's a failure
}
