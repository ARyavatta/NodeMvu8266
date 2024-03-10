#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <WiFiClientSecure.h> // Include WiFiClient for network connections

// WiFi credentials
const char* ssid = "RoyNiketan_2.4";
const char* password = "Assassin#0";

// Server URL for sending temperature and humidity data
const char* serverUrl = "https://homearoy.azurewebsites.net/api/TempHumid/Temperature";

// DHT sensor setup
#define DHTPIN D4          // DHT11 data pin is connected to D4 of ESP8266
#define DHTTYPE DHT11      // DHT11 sensor is being used
DHT dht(DHTPIN, DHTTYPE);

// Create a WiFiClient object
WiFiClientSecure wifiClient;

// Function Prototypes
void connectToWiFi();
void sendTempHumidData(float temperature, float humidity);

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
  dht.begin();          // Initialize DHT sensor
  
  connectToWiFi();      // Connect to WiFi network

  wifiClient.setInsecure();
}

void loop() {
  float humidity = dht.readHumidity();          // Read humidity (percent)
  float temperature = dht.readTemperature();    // Read temperature (Celsius)

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Print temperature and humidity data to the Serial Monitor
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%  Temperature: ");
    Serial.print(temperature);
    Serial.println("°C ");

    // Send temperature and humidity data to the server
    sendTempHumidData(temperature, humidity);
  }

  Serial.println("Entering deep sleep for 15 minutes...");
  ESP.deepSleep(15 * 60 * 1000000);
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void sendTempHumidData(float temperature, float humidity) {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi(); // Ensure WiFi is connected
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, serverUrl); // Use WiFiClientSecure for HTTPS connection
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"temperature\":" + String(temperature, 2) + ",\"humidity\":" + String(humidity, 2) + ",\"macAddress\":\"" + WiFi.macAddress() + "\"}";

    Serial.println("Sending JSON payload:");
    Serial.println(jsonPayload);

    int httpResponseCode = http.POST(jsonPayload);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
      String responsePayload = http.getString();
      Serial.println("HTTP Response payload: ");
      Serial.println(responsePayload);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

