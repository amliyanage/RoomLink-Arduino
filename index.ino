#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "nubiav60";
const char* password = "2345678910";
const char* firebaseURL = "https://room-controller-455409-default-rtdb.firebaseio.com/relayArry.json";

// Relay pins: GPIO15, GPIO2, GPIO4, GPIO5
const int relayPins[] = {15, 2, 4, 5};  // relay1 to relay4

// Optional LED indicators
const int redLED = 19;
const int blueLED = 21;
const int greenLED = 23;

void setup() {
  Serial.begin(115200);

  // Setup relay pins
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);  // OFF initially (active LOW relays)
  }

  // Setup indicator LEDs
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  digitalWrite(redLED, HIGH);   // Waiting for WiFi
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to WiFi!");
  digitalWrite(redLED, HIGH);
  digitalWrite(blueLED, HIGH);  // WiFi connected
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(firebaseURL);
    int httpCode = http.GET();

    if (httpCode == 200) {
      digitalWrite(greenLED, HIGH);  // Firebase OK
      String payload = http.getString();
      Serial.println("Firebase Response:");
      Serial.println(payload);

      // Parse JSON response
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, payload);
      if (!error) {
        const char* states[4] = {
          doc["relay1"], doc["relay2"], doc["relay3"], doc["relay4"]
        };

        // Set relay pins
        for (int i = 0; i < 4; i++) {
          if (states[i] && strcmp(states[i], "ON") == 0) {
            digitalWrite(relayPins[i], LOW);  // ON (active LOW)
            Serial.printf("Relay %d ON\n", i + 1);
          } else {
            digitalWrite(relayPins[i], HIGH); // OFF
            Serial.printf("Relay %d OFF\n", i + 1);
          }
        }
      } else {
        Serial.println("JSON parse error");
      }
    } else {
      Serial.printf("HTTP Error: %d\n", httpCode);
      digitalWrite(greenLED, LOW);  // Firebase not reachable
    }

    http.end();
  }

  delay(2000);  // Wait before next check
}
