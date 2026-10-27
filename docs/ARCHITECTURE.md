# Architecture and credentials

```text
MQ gas sensor -> ESP32 sampling/filtering -> safety state machine
                                             |- LEDs + buzzer
                                             |- fan relay (danger/critical)
                                             |- servo valve (latched closed at critical)
                                             |- ThingSpeak telemetry (optional)
                                             `- Twilio SMS (optional, rate limited)
```

## Request flow

1. Every 500 ms the ESP32 takes an ADC sample and smooths it with a small moving filter.
2. Three consecutive samples are required before changing state. Hysteresis prevents fast state flapping near a threshold.
3. Safe, warning, danger, and critical states drive the local hardware. Critical closes the valve and leaves it closed. Only a person on site can reopen it: readings must be safe and the configured reset button must be held for three seconds.
4. When Wi-Fi is available, ThingSpeak receives a reading no more than once every 20 seconds. Danger and critical transitions may send a Twilio SMS, subject to a 10-minute cooldown.

## Credentials and tokens

The original sketches embedded a Wi-Fi password, a ThingSpeak write key, a Blynk token, and Twilio credentials. These are secrets and must be treated as compromised because they were committed publicly. Rotate or revoke all of them in their provider dashboards, change the Wi-Fi password if it is still used, and scrub repository history separately if needed.

The maintained firmware reads new credentials from `include/config.h`, which is not tracked. It uses safe defaults until that file is added. It uses HTTPS for ThingSpeak and Twilio. The Twilio connection currently uses `setInsecure()` for compatibility; production hardware should pin a trusted CA certificate before connecting to an external service. Blynk was removed from the maintained path because the previous dashboard/template configuration was undocumented; it can be added back only after its template, event, and authorization settings are documented.

The GitHub repository itself is public and has no application login or authorization flow. The only runtime authentication is Wi-Fi association and the optional cloud-service credentials described above.
