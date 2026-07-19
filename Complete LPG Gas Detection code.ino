#define BLYNK_TEMPLATE_ID "TMPL3M8lMwwMV"
#define BLYNK_TEMPLATE_NAME "Gas Detection"
#define BLYNK_AUTH_TOKEN "bP25zzxUyYz72__0f0IdGeowpv0o_t_1"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include "mbedtls/base64.h"

char ssid[] = "Akash";
char pass[] = "120130140";

// ThingSpeak
String thingSpeakApiKey = "X1HD7DISAMDCTD0G";

// Twilio
String accountSID = "ACe0794fb2da82ea507ba6d0ade2499896";
String apiKey     = "SKd2c63ed64f4799e161a643502a747934";
String apiSecret  = "Z96YeM35hfiVsDxjVn3iRfhxhjv29HHo";
String twilioFrom = "+16416306026";
String toNumber   = "+919492422314";

// Pins
int gasSensor = 34;
int buzzerPin = 26;
int fanPin = 27;
int servoPin = 25;
int greenLedPin = 32;
int redLedPin = 33;

// Threshold
int threshold = 3800;

// Servo
Servo myServo;

// Alert control
bool alertSent = false;

// Manual Blynk controls
bool manualServoOff = false;
bool manualFanOn = false;

// Continuous high reading counter 
int highCount = 0;

// ---------- Base64 encode for Twilio ----------
String base64Encode(String input) {
  size_t outputLen = 0;
  unsigned char output[256];

  mbedtls_base64_encode(
    output,
    sizeof(output),
    &outputLen,
    (const unsigned char*)input.c_str(),
    input.length()
  );

  return String((char*)output).substring(0, outputLen);
}

// ---------- Send SMS using Twilio ----------
void sendTwilioSMS(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. SMS not sent.");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  String url = "https://api.twilio.com/2010-04-01/Accounts/" + accountSID + "/Messages.json";

  Serial.println("Sending SMS through Twilio...");

  if (!http.begin(client, url)) {
    Serial.println("HTTP begin failed for Twilio");
    return;
  }

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String authString = apiKey + ":" + apiSecret;
  String authBase64 = base64Encode(authString);
  http.addHeader("Authorization", "Basic " + authBase64);

  String postData = "To=" + toNumber +
                    "&From=" + twilioFrom +
                    "&Body=" + message;

  int httpCode = http.POST(postData);

  Serial.print("Twilio HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("Twilio Response:");
    Serial.println(response);
  } else {
    Serial.print("Twilio Error: ");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, LOW);

  pinMode(buzzerPin, OUTPUT);
  pinMode(fanPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);
  digitalWrite(fanPin, LOW);

  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
  myServo.write(0);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (Blynk.connected()) {
    Serial.println("Blynk Connected!");
  } else {
    Serial.println("Blynk Not Connected!");
  }
}

void loop() {
  Blynk.run();

  int gasValue = analogRead(gasSensor);

  Serial.println("----------------------------------");
  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  Blynk.virtualWrite(V0, gasValue);

  if (gasValue > threshold) {
    highCount++;
  } else {
    highCount = 0;
  }

  Serial.print("High Count: ");
  Serial.println(highCount);

  // Send to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = "http://api.thingspeak.com/update?api_key=" + thingSpeakApiKey +
                 "&field1=" + String(gasValue) +
                 "&field2=" + String(highCount >= 3 ? 1 : 0);

    Serial.println("Sending data to ThingSpeak...");

    http.begin(url);
    int httpCode = http.GET();

    Serial.print("ThingSpeak Response Code: ");
    Serial.println(httpCode);

    http.end();
  }

  // Automatic gas detection
  if (highCount >= 3) {
    Serial.println("GAS LEAK DETECTED!");

    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fanPin, HIGH);
    myServo.write(90);

    if (!alertSent) {
      Blynk.logEvent("gas_detection", "Gas Leakage Detected!");
      Serial.println("Alert Sent to Blynk!");

      sendTwilioSMS("ALERT: LPG Leak Detected. Valve OFF, Fan ON. Check immediately.");
      Serial.println("Twilio SMS function called!");

      alertSent = true;
    }

  } else {
    Serial.println("Safe Environment");

    digitalWrite(greenLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
    digitalWrite(buzzerPin, LOW);

    // Manual fan control in safe condition
    if (manualFanOn) {
      digitalWrite(fanPin, HIGH);
    } else {
      digitalWrite(fanPin, LOW);
    }

    // Manual servo control in safe condition
    if (manualServoOff) {
      myServo.write(90);
    } else {
      myServo.write(0);
    }

    if (alertSent && highCount == 0) {
      Serial.println("Gas back to normal. Alert reset.");
      alertSent = false;
    }
  }

  //delay(15000);
  for (int i = 0; i < 150; i++) {
  Blynk.run();
  delay(100);
}
}

// Manual fan control from Blynk - V1
BLYNK_WRITE(V1) {
  int value = param.asInt();
  manualFanOn = value;

  digitalWrite(fanPin, manualFanOn ? HIGH : LOW);

  Serial.print("Manual Fan from Blynk: ");
  Serial.println(manualFanOn ? "ON" : "OFF");
}

// Manual servo valve OFF from Blynk - V2
BLYNK_WRITE(V2) {
  int value = param.asInt();
  manualServoOff = value;

  if (manualServoOff) {
    myServo.write(90);
    Serial.println("Manual Servo: Valve CLOSED from Blynk");
  } else {
    myServo.write(0);
    Serial.println("Manual Servo: Valve OPEN / Auto restored");
  }
}