# Smart LPG Gas Detection System

ESP32 firmware for MQ-series gas monitoring with local safety actions and optional cloud telemetry/SMS alerts.

## What is in this repository

```text
src/main.cpp              Maintained ESP32 firmware
include/config.example.h  Safe configuration template (copy to config.h locally)
platformio.ini            Reproducible PlatformIO build configuration
docs/SETUP.md             Wiring, calibration, and upload steps
docs/ARCHITECTURE.md      Data flow and credential handling
```

The project reads an MQ sensor, then applies four safety levels:

| Level | Behaviour |
| --- | --- |
| Safe | Green LED; monitoring only. |
| Warning | Audible/visual warning. |
| Danger | Warning, ventilation fan, and rate-limited alert. |
| Critical | All danger actions plus a latched gas-valve close command; a local safe-state reset is required to reopen it. |

Start with [the setup guide](docs/SETUP.md). This hardware controls safety-relevant equipment: test the sensor, servo direction, relay wiring, and threshold values before connecting it to an LPG supply.

## Security

No active credentials belong in the repository. Copy `include/config.example.h` to `include/config.h` and keep it local. See [architecture and credential guidance](docs/ARCHITECTURE.md), including the necessary rotation of secrets exposed in the historical sketches.

The two original `.ino` files are retired stubs that deliberately stop Arduino-IDE uploads. Build `src/main.cpp` with PlatformIO instead.
