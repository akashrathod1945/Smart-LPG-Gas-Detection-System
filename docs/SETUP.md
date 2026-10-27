# Device setup

## 1. Safety first

This is a monitoring and response aid, not a certified gas-safety device. Test it only in a controlled, ventilated environment. The fan and valve must be driven through correctly rated relays or drivers; never connect mains equipment directly to the ESP32. Have a qualified person validate the valve mechanism before relying on it.

The firmware starts with the servo in its configured **closed** position and latches it closed after a critical reading. It deliberately does not reopen the gas supply automatically.

## 2. Wiring

| Part | ESP32 GPIO | Notes |
| --- | --- | --- |
| MQ-series sensor analogue output | 34 | Use a safe 3.3 V analogue level. Some MQ boards output 5 V and need a divider. |
| Buzzer driver | 26 | Use a transistor/driver if the buzzer current exceeds a GPIO limit. |
| Fan relay driver | 27 | Use an isolated, correctly rated relay/driver. |
| Servo signal | 25 | Power the servo from a suitable external supply; share ground. |
| Green LED | 32 | Include a current-limiting resistor. |
| Red LED | 33 | Include a current-limiting resistor. |
| Valve-reset button | 14 | Momentary, normally open button to GND; hold for 3 seconds only after the area is safe. |

## 3. Configure and build

1. Install [PlatformIO](https://platformio.org/).
2. Copy `include/config.example.h` to `include/config.h`.
3. Enter Wi-Fi credentials. Enable only the cloud integrations you intend to use.
4. Build and upload with `pio run --target upload`; monitor serial output with `pio device monitor`.

`config.h` is intentionally ignored by Git. Never edit the example file with real values.

## 4. Calibrate before use

Let the MQ sensor warm up according to its datasheet, record clean-air values and repeatable test values, then set `WARNING_THRESHOLD`, `DANGER_THRESHOLD`, and `CRITICAL_THRESHOLD` in `src/main.cpp`. These ADC values are **not** ppm measurements and vary with sensor type, heater time, supply voltage, and environment.

Test every output separately first. Confirm servo directions before connecting a real gas valve. Set `SERVO_OPEN_ANGLE` and `SERVO_CLOSED_ANGLE` for the installed mechanism. The device starts with its valve command closed; after a safe reading, a person on site must hold the reset button for three seconds to allow it to open.
