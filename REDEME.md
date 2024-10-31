Here is the `README.md` file in English, formatted as requested:

---

# Nature-Exploration-Translator
This project is an interactive system using an ESP32, Force-Sensitive Resistors (FSR), an NFC reader, RGB LEDs, and audio output. When the FSR is pressed, the ESP32 reads the sensor value and controls the LED indicators to provide visual feedback. If an NFC card is detected, the system calls the OpenAI API to retrieve a short description and plays it through the I2S audio output.

## Components Used
- **ESP32**: Microcontroller for managing sensors, NFC, LEDs, and audio.
- **Force-Sensitive Resistors (FSR)**: Detects touch pressure and triggers corresponding actions.
- **RGB LEDs**: Provides visual feedback based on specific conditions.
- **MFRC522 NFC Reader**: Reads NFC tags to obtain relevant data.
- **I2S Audio Amplifier** (e.g., MAX98357A): Plays audio descriptions returned by OpenAI.
- **Fixed Resistor (R_fixed)**: Used with each FSR to create a voltage divider circuit.

## Circuit Diagram
1. **FSR with Voltage Divider**: Connect each FSR with a fixed resistor in a voltage divider circuit to an ADC pin on the ESP32.
2. **LEDs**: Connect each RGB LED to specified GPIO pins on the ESP32.
3. **NFC Module**: Connect the MFRC522 module to the ESP32’s SPI pins.
4. **Audio Amplifier**: Connect the ESP32’s I2S pins (BCK, WS, and DO) to the audio amplifier.

## Project Setup and Working Principle

1. **Install Required Libraries**:
   - Install libraries: `MFRC522` (for NFC), `ArduinoJson` (for parsing API responses), and `BH1750` (for optional light sensing).
   - Ensure correct configuration if using PlatformIO for uploading code.

2. **Configure Wi-Fi and OpenAI API**:
   - In the code, update `ssid`, `password`, and `openaiApiKey` with your Wi-Fi credentials and OpenAI API key.

3. **Connect Components**:
   - Use a voltage divider circuit to connect each FSR to the analog input pins of the ESP32.
   - Connect LEDs to the GPIO pins defined in the code.
   - Connect the NFC reader to the SPI pins and the I2S audio amplifier to the I2S pins of the ESP32.

4. **Upload Code**:
   - Use PlatformIO or Arduino IDE to upload the code to the ESP32.

## Code Overview
- **Wi-Fi Connection**: Connects the ESP32 to the internet for accessing the OpenAI API.
- **NFC Reading**: Detects an NFC card, stores the data, and associates each card with an FSR.
- **FSR Detection**: Reads the FSR values and, based on detected pressure, activates LEDs and triggers an OpenAI API request to get a description.
- **OpenAI API Request**: Sends a prompt to OpenAI, retrieves a brief description of the detected NFC card, and plays it as audio.
- **LED Control**: Controls the LEDs to show green or red based on conditions (such as pressing the same FSR repeatedly).

## How to Use
1. **NFC Card Detection**: Place an NFC card on the reader; the system will store the card data and associate it with an FSR.
2. **Using the Force-Sensitive Resistor**:
   - Press an FSR. If it’s the first press, the LED will light up green and an OpenAI description request will be triggered.
   - If you press the same FSR again, the LED will briefly light up green.
   - If you press a different FSR after the first one, the LED will light up red.
3. **Audio Output**: If a valid description is received from OpenAI, the audio amplifier will play the description.

## Troubleshooting
- Ensure NFC cards are compatible and properly placed on the reader.
- Check Wi-Fi credentials and OpenAI API key for internet access.
- If LEDs or audio aren’t working as expected, check connections and GPIO pin configurations.

## Future Improvements
- Add more FSRs or NFC cards for enhanced interactions.
- Integrate additional APIs or sensors to expand the project’s functionality.

---

This `README.md` file provides a quick-start guide for the project, explaining how to set up, connect components, upload code, and operate the system. This documentation allows others to understand the working principle and the function of each component in the project.
