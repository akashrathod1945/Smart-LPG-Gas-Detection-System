#pragma once

// Copy this file to include/config.h and fill in only the integrations you use.
// config.h is ignored by Git so credentials never enter a commit.

constexpr char WIFI_SSID[] = "replace-with-wifi-name";
constexpr char WIFI_PASSWORD[] = "replace-with-wifi-password";

// Set to 1 only after supplying a ThingSpeak write key.
#define ENABLE_THINGSPEAK 0
constexpr char THINGSPEAK_WRITE_KEY[] = "";

// Twilio uses HTTP Basic authentication. Create a restricted API key and
// secret in Twilio; do not use an account password or commit either value.
#define ENABLE_TWILIO 0
constexpr char TWILIO_ACCOUNT_SID[] = "";
constexpr char TWILIO_API_KEY[] = "";
constexpr char TWILIO_API_SECRET[] = "";
constexpr char TWILIO_FROM[] = "";
constexpr char ALERT_PHONE_NUMBER[] = "";
