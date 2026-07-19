#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

const char* ssid = "Delightboyshostel-201-2G";
const char* password = "delight-201-202-203";

String apiKey = "X1HD7DISAMDCTD0G";

// Pin definitions
int gasSensor = 34;
int buzzerPin = 26;
int fanPin = 27;
int servoPin = 25;

int threshold = 3000;

Servo regulatorServo;

void setup() {

  Serial.begin(115200);

  pinMode(buzzerPin, OUTPUT);
  pinMode(fanPin, OUTPUT);

  regulatorServo.attach(servoPin);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

}

void loop() {

  int gasValue = analogRead(gasSensor);

  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  if (gasValue > threshold) {

    Serial.println("Gas Detected!");

    digitalWrite(buzzerPin, HIGH);  // buzzer ON
    digitalWrite(fanPin, HIGH);     // fan ON

    regulatorServo.write(90);       // close regulator

  } else {

    digitalWrite(buzzerPin, LOW);   // buzzer OFF
    digitalWrite(fanPin, LOW);      // fan OFF

    regulatorServo.write(0);        // normal position

  }

  // Send data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    String url = "http://api.thingspeak.com/update?api_key=" + apiKey + "&field1=" + String(gasValue);

    http.begin(url);

    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("Data uploaded to ThingSpeak");
    } else {
      Serial.println("Error uploading data");
    }

    http.end();
  }

  delay(15000); // ThingSpeak requires 15 sec delay
}