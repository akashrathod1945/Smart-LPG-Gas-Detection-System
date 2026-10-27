#pragma once

// Safe compile-time defaults. Create config.h from config.example.h to connect
// a device; config.h takes precedence and stays outside version control.
constexpr char WIFI_SSID[] = "";
constexpr char WIFI_PASSWORD[] = "";

#define ENABLE_THINGSPEAK 0
constexpr char THINGSPEAK_WRITE_KEY[] = "";

#define ENABLE_TWILIO 0
constexpr char TWILIO_ACCOUNT_SID[] = "";
constexpr char TWILIO_API_KEY[] = "";
constexpr char TWILIO_API_SECRET[] = "";
constexpr char TWILIO_FROM[] = "";
constexpr char ALERT_PHONE_NUMBER[] = "";
