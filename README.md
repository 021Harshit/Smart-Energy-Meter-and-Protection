# Smart-Energy-Meter-and-Protection

# Smart Energy Meter with Intelligent Load Protection

An Arduino Uno based smart energy meter simulated in Tinkercad. It measures voltage and current, calculates real-time power and energy consumption, and automatically disconnects the load when unsafe levels are detected.

## Features

- **Voltage monitoring:** 180–260 V (potentiometer-simulated input)
- **Current monitoring:** 0–15 A (potentiometer-simulated input)
- **Accurate readings:** 20-sample averaged ADC values to reduce noise
- **Real-time calculations:** Power (W) and energy (Wh/kWh)
- **Automatic protection:** Relay cuts off the load when voltage > 250 V or current > 10 A
- **Fault handling:** Red/green status LEDs, buzzer alarm, and I2C LCD fault display
- **Manual reset:** Push button re-enables the relay only after readings return to safe levels

## Components

| Component | Purpose |
|---|---|
| Arduino Uno | Main controller |
| 2 × Potentiometer | Simulate voltage and current sensors (A0, A1) |
| 16x2 I2C LCD | Displays voltage, current, power, energy and status |
| Relay + NPN transistor + diode | Load switching with flyback protection |
| Bulb + power supply | Load |
| Red / Green LEDs | Fault / normal indicators |
| Piezo buzzer | Fault alarm |
| Push button | Manual reset |
| Resistors | LED and transistor base current limiting |

## Working

1. The Arduino reads the voltage and current inputs and averages 20 ADC samples each.
2. Values are scaled to real units (V, A), then power `P = V × I` and energy `E = P × time` are computed.
3. Readings are shown on the I2C LCD. The green LED is on during normal operation.
4. If voltage exceeds **250 V** or current exceeds **10 A**, the relay turns off, the red LED and buzzer activate, and a fault message is displayed.
5. After the fault, pressing the reset button re-enables the relay only if the readings are back within safe limits.

## Tech Stack

Arduino Uno, Embedded C/C++, Tinkercad Circuits, I2C (LiquidCrystal_I2C)

## Thresholds

| Parameter | Range | Cut-off |
|---|---|---|
| Voltage | 180–260 V | > 250 V |
| Current | 0–15 A | > 10 A |
