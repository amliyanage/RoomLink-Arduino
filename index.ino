#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Firebase Credentials (Replace with your Database URL)
const char* firebaseURL = "https://room-controller-455409-default-rtdb.firebaseio.com/relay.json";

const int relayPin = 4; // Simulated relay (LED in Wokwi)

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Start with relay OFF

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    Serial.println("Checking Firebase...");

    // Send GET request to Firebase
    http.begin(firebaseURL);
    int httpCode = http.GET();

    if (httpCode == 200) { // If request is successful
      String payload = http.getString();
      Serial.println("Firebase Response: " + payload);

      if (payload.indexOf("ON") > 0) {
        digitalWrite(relayPin, HIGH);
        Serial.println("Relay ON");
      } else {
        digitalWrite(relayPin, LOW);
        Serial.println("Relay OFF");
      }
    } else {
      Serial.println("Error in HTTP Request");
    }

    http.end();
  }

  delay(2000); // Check Firebase every 2 seconds
}
