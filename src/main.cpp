#include <Arduino.h>
#include <ctype.h>
#include <ESP32Servo.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <mbedtls/base64.h>

#if __has_include("config.h")
#include "config.h"
#else
#include "config.defaults.h"
#endif

namespace {

// Wiring: MQ analogue output -> GPIO34, buzzer -> 26, fan relay -> 27,
// servo signal -> 25, green LED -> 32, red LED -> 33.
constexpr uint8_t GAS_SENSOR_PIN = 34;
constexpr uint8_t BUZZER_PIN = 26;
constexpr uint8_t FAN_PIN = 27;
constexpr uint8_t SERVO_PIN = 25;
constexpr uint8_t GREEN_LED_PIN = 32;
constexpr uint8_t RED_LED_PIN = 33;
constexpr uint8_t VALVE_RESET_BUTTON_PIN = 14;  // Momentary button wired to GND.

// Calibrate these values for the installed sensor, in its final enclosure.
constexpr int WARNING_THRESHOLD = 1600;
constexpr int DANGER_THRESHOLD = 2400;
constexpr int CRITICAL_THRESHOLD = 3200;
constexpr int THRESHOLD_HYSTERESIS = 100;
constexpr uint8_t REQUIRED_CONSECUTIVE_READINGS = 3;

constexpr uint8_t SERVO_OPEN_ANGLE = 0;
constexpr uint8_t SERVO_CLOSED_ANGLE = 90;
constexpr bool START_WITH_VALVE_CLOSED = true;

constexpr unsigned long SAMPLE_INTERVAL_MS = 500;
constexpr unsigned long WIFI_RETRY_INTERVAL_MS = 10000;
constexpr unsigned long THINGSPEAK_INTERVAL_MS = 20000;  // ThingSpeak minimum: 15 s.
constexpr unsigned long SMS_COOLDOWN_MS = 10UL * 60UL * 1000UL;
constexpr unsigned long VALVE_RESET_HOLD_MS = 3000;

enum class SafetyLevel : uint8_t { Safe, Warning, Danger, Critical };

Servo valveServo;
SafetyLevel level = SafetyLevel::Safe;
SafetyLevel pendingLevel = SafetyLevel::Safe;
uint8_t pendingCount = 0;
bool valveClosed = START_WITH_VALVE_CLOSED;
int filteredGasValue = 0;
unsigned long lastSampleAt = 0;
unsigned long lastWifiAttemptAt = 0;
unsigned long lastThingSpeakAt = 0;
unsigned long lastSmsAt = 0;
unsigned long valveResetPressedAt = 0;

const char *levelName(SafetyLevel value) {
  switch (value) {
    case SafetyLevel::Safe: return "safe";
    case SafetyLevel::Warning: return "warning";
    case SafetyLevel::Danger: return "danger";
    case SafetyLevel::Critical: return "critical";
  }
  return "unknown";
}

SafetyLevel rawLevelFor(int value) {
  if (value >= CRITICAL_THRESHOLD) return SafetyLevel::Critical;
  if (value >= DANGER_THRESHOLD) return SafetyLevel::Danger;
  if (value >= WARNING_THRESHOLD) return SafetyLevel::Warning;
  return SafetyLevel::Safe;
}

// A level may decrease only after crossing below its threshold plus a margin.
SafetyLevel levelWithHysteresis(int value) {
  if (level == SafetyLevel::Critical && value >= CRITICAL_THRESHOLD - THRESHOLD_HYSTERESIS) return level;
  if (level == SafetyLevel::Danger && value >= DANGER_THRESHOLD - THRESHOLD_HYSTERESIS) return level;
  if (level == SafetyLevel::Warning && value >= WARNING_THRESHOLD - THRESHOLD_HYSTERESIS) return level;
  return rawLevelFor(value);
}

void connectWifiIfNeeded() {
  if (strlen(WIFI_SSID) == 0) return;
  if (WiFi.status() == WL_CONNECTED || millis() - lastWifiAttemptAt < WIFI_RETRY_INTERVAL_MS) return;
  lastWifiAttemptAt = millis();
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

String urlEncode(const String &input) {
  const char *hex = "0123456789ABCDEF";
  String encoded;
  for (size_t i = 0; i < input.length(); ++i) {
    const uint8_t c = static_cast<uint8_t>(input[i]);
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += static_cast<char>(c);
    } else {
      encoded += '%';
      encoded += hex[c >> 4];
      encoded += hex[c & 0x0F];
    }
  }
  return encoded;
}

String base64Encode(const String &input) {
  unsigned char output[257] = {};
  size_t outputLength = 0;
  if (mbedtls_base64_encode(output, sizeof(output), &outputLength,
                            reinterpret_cast<const unsigned char *>(input.c_str()), input.length()) != 0) {
    return "";
  }
  output[outputLength] = '\0';
  return String(reinterpret_cast<char *>(output));
}

void sendSms(const String &message) {
#if ENABLE_TWILIO
  if (WiFi.status() != WL_CONNECTED || strlen(TWILIO_ACCOUNT_SID) == 0 || strlen(TWILIO_API_KEY) == 0) return;

  WiFiClientSecure client;
  client.setInsecure();  // Replace with a pinned CA certificate for production use.
  HTTPClient http;
  const String url = "https://api.twilio.com/2010-04-01/Accounts/" + String(TWILIO_ACCOUNT_SID) + "/Messages.json";
  if (!http.begin(client, url)) return;

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Basic " + base64Encode(String(TWILIO_API_KEY) + ":" + TWILIO_API_SECRET));
  const String body = "To=" + urlEncode(ALERT_PHONE_NUMBER) + "&From=" + urlEncode(TWILIO_FROM) +
                      "&Body=" + urlEncode(message);
  const int status = http.POST(body);
  Serial.printf("Twilio status: %d\\n", status);
  http.end();
#else
  (void)message;
#endif
}

void uploadThingSpeak() {
#if ENABLE_THINGSPEAK
  if (WiFi.status() != WL_CONNECTED || strlen(THINGSPEAK_WRITE_KEY) == 0) return;
  HTTPClient http;
  const String url = "https://api.thingspeak.com/update?api_key=" + String(THINGSPEAK_WRITE_KEY) +
                     "&field1=" + String(filteredGasValue) +
                     "&field2=" + String(static_cast<int>(level)) +
                     "&field3=" + String(valveClosed ? 1 : 0);
  if (http.begin(url)) {
    Serial.printf("ThingSpeak status: %d\\n", http.GET());
    http.end();
  }
#endif
}

void applyOutputs() {
  const bool alarm = level == SafetyLevel::Warning || level == SafetyLevel::Danger || level == SafetyLevel::Critical;
  const bool fanOn = level == SafetyLevel::Danger || level == SafetyLevel::Critical;
  if (level == SafetyLevel::Critical) valveClosed = true;  // Latch shut; never reopen automatically.

  digitalWrite(GREEN_LED_PIN, level == SafetyLevel::Safe ? HIGH : LOW);
  digitalWrite(RED_LED_PIN, alarm ? HIGH : LOW);
  digitalWrite(BUZZER_PIN, alarm ? HIGH : LOW);
  digitalWrite(FAN_PIN, fanOn ? HIGH : LOW);
  valveServo.write(valveClosed ? SERVO_CLOSED_ANGLE : SERVO_OPEN_ANGLE);
}

void setLevel(SafetyLevel nextLevel) {
  if (nextLevel == level) return;
  level = nextLevel;
  Serial.printf("Safety level changed to %s (reading %d)\\n", levelName(level), filteredGasValue);
  applyOutputs();

  if (level == SafetyLevel::Danger || level == SafetyLevel::Critical) {
    const unsigned long now = millis();
    if (now - lastSmsAt >= SMS_COOLDOWN_MS || lastSmsAt == 0) {
      sendSms("LPG warning: " + String(levelName(level)) + " level detected. Reading: " + String(filteredGasValue) +
              ". Check the site immediately.");
      lastSmsAt = now;
    }
  }
}

void sampleSensor() {
  const int currentReading = analogRead(GAS_SENSOR_PIN);
  filteredGasValue = filteredGasValue == 0 ? currentReading : (filteredGasValue * 3 + currentReading) / 4;
  const SafetyLevel candidate = levelWithHysteresis(filteredGasValue);

  if (candidate != pendingLevel) {
    pendingLevel = candidate;
    pendingCount = 1;
  } else if (pendingCount < REQUIRED_CONSECUTIVE_READINGS) {
    ++pendingCount;
  }
  if (pendingCount >= REQUIRED_CONSECUTIVE_READINGS) setLevel(pendingLevel);

  Serial.printf("Gas=%d filtered=%d level=%s valve=%s\\n", currentReading, filteredGasValue,
                levelName(level), valveClosed ? "closed" : "open");
}

// The valve can be reopened only by holding a local button while readings are
// safe. This prevents an old cloud command or a transient reading from opening
// a valve after a critical alarm.
void handleValveReset() {
  if (level != SafetyLevel::Safe || digitalRead(VALVE_RESET_BUTTON_PIN) == HIGH) {
    valveResetPressedAt = 0;
    return;
  }
  if (valveResetPressedAt == 0) {
    valveResetPressedAt = millis();
    return;
  }
  if (valveClosed && millis() - valveResetPressedAt >= VALVE_RESET_HOLD_MS) {
    valveClosed = false;
    valveResetPressedAt = 0;
    applyOutputs();
    Serial.println("Valve reset accepted after local safety check.");
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(VALVE_RESET_BUTTON_PIN, INPUT_PULLUP);

  valveServo.setPeriodHertz(50);
  valveServo.attach(SERVO_PIN, 500, 2400);
  applyOutputs();
  WiFi.mode(WIFI_STA);
  connectWifiIfNeeded();
}

void loop() {
  const unsigned long now = millis();
  connectWifiIfNeeded();
  handleValveReset();

  if (now - lastSampleAt >= SAMPLE_INTERVAL_MS) {
    lastSampleAt = now;
    sampleSensor();
  }
  if (now - lastThingSpeakAt >= THINGSPEAK_INTERVAL_MS) {
    lastThingSpeakAt = now;
    uploadThingSpeak();
  }
}
