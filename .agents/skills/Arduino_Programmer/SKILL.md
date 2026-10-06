---
name: Arduino_Programmer
description: Assists with Arduino development by identifying the board architecture, researching sensor specifications, finding git repositories/documentation, and implementing robust microcontroller code.
---
# Arduino Programmer Skill

Follow this structured process when developing for Arduino:

1. **Architecture & Board Identification**:
   - Before writing any code, explicitly ask the user for their board details and microchip architecture (e.g., AVR/Uno/Mega, SAMD/Zero, ESP8266, ESP32, STM32, RP2040).
   - Clarify the voltage requirements (3.3V vs 5V) to ensure compatibility with sensors.

2. **Sensor & Component Investigation**:
   - Catalog all sensors, actuators, and interface components (e.g., I2C, SPI, UART, analog, digital) connected to the system.
   - Look up datasheets and specifications to understand pinouts and power limits.

3. **Documentation & Library Research**:
   - Search git repositories, official Arduino documentation, and online sources to find the most stable and appropriate libraries for the components.
   - Reference example codes and API signatures to verify correct library usage.

4. **Hardware Hookup & Pin Allocation**:
   - Document the wiring scheme, showing pin connections, power lines, and pull-up/pull-down resistors where necessary.

5. **Implementation & Best Practices**:
   - Write clean, non-blocking Arduino code (preferring `millis()` over `delay()` for timing-sensitive loops).
   - Implement proper initialization in `setup()` and optimize loop performance in `loop()`.
   - Organize custom libraries or helper classes when building complex logic.
