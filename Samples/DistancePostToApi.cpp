#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// Function Prototypes
float getDistanceMeasurement();
void sendDistanceData(float distance);
void sendDistanceData(float* distances, int count);
void connectToWiFi();
void disconnectWiFiIfApplicable();
void handleServerResponse(const String& response);

// WiFi credentials and server URL
const char* ssid = "RoyNiketan_2.4";
const char* password = "Assassin#0";
const char* serverUrl = "https://atipre.azurewebsites.net/api/Esp8266/Distances";

// GPIO pins for the ultrasonic sensor
const int trigPin = D1;
const int echoPin = D2;

WiFiClientSecure client;

unsigned long lastMeasurementTime = 0;
int measurementInterval = 1000; // Initialized to 1-second interval
float distanceMeasurements[60]; // Buffer for batch measurements
int measurementCount = 0; // Counter for batch measurements

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  connectToWiFi(); // Connect to WiFi at startup
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastMeasurementTime >= measurementInterval) {
    lastMeasurementTime = currentTime;
    float distance = getDistanceMeasurement();
    Serial.println("Distance: " + String(distance) + " cm");

    if (measurementInterval == 1000) { // For real-time sending
      sendDistanceData(distance);
    } else { // For batch sending
      distanceMeasurements[measurementCount++] = distance;
      if (measurementCount >= sizeof(distanceMeasurements) / sizeof(distanceMeasurements[0])) {
        sendDistanceData(distanceMeasurements, measurementCount);
        measurementCount = 0; // Reset count after sending
      }
    }
  }

  delay(10); // Small delay to prevent blocking
}

float getDistanceMeasurement() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2; // Calculate distance in cm
}

void sendDistanceData(float distance) {
  if (WiFi.status() != WL_CONNECTED) connectToWiFi();

  DynamicJsonDocument doc(256);
  doc["deviceMac"] = WiFi.macAddress();
  doc["distance"] = distance;

  String payload;
  serializeJson(doc, payload);

  HTTPClient http;
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    String response = http.getString();
    handleServerResponse(response);
  }
  http.end();
}

void sendDistanceData(float* distances, int count) {
  if (WiFi.status() != WL_CONNECTED) connectToWiFi();

  DynamicJsonDocument doc(1024);
  JsonArray measurements = doc.createNestedArray("measurements");
  for (int i = 0; i < count; i++) {
    measurements.add(distances[i]);
  }
  doc["deviceMac"] = WiFi.macAddress();

  String payload;
  serializeJson(doc, payload);

  HTTPClient http;
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    String response = http.getString();
    handleServerResponse(response);
  }
  http.end();
}

void handleServerResponse(const String& response) {
  DynamicJsonDocument doc(256);
  deserializeJson(doc, response);
  if (doc.containsKey("interval")) {
    int interval = doc["interval"];
    measurementInterval = (interval == 1) ? 1000 : 15000; // Adjust interval based on server response
  }
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void disconnectWiFiIfApplicable() {
  if (measurementInterval != 1000 && WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
    Serial.println("Disconnected from WiFi to conserve power.");
  }
}
