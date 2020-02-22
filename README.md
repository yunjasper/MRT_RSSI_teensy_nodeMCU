# McGill Rocket Team Avionics 2020 - RSSI receiver to nodeMCU v0.9

Contains the code to be run on the Teensy 3.5 which interfaces with the following:<br>

- LCD 16x2
- Buzzer
- nodeMCU v0.9
- XTEND RF Module (not yet implemented)

## Teensy 3.5 to nodeMCU - Pinout Configuration:
- pin 0 (RX1) to voltage divider (1 kOhm and 560 Ohm), then to nodeMCU TX
- pin 1 (TX1) to nodeMCU RX
- VIN to 4.40 V power supply
- GND to GND
- 3.3V to nodeMCU 3V

## Teensy 3.5 to LCD and Buzzer - Pinout Configuration:
- pin 4 to LCD RS
- pin 5 to LCD EN
- pin 6 to LCD D4
- pin 7 to LCD D5
- pin 8 to LCD D6
- pin 9 to LCD D7
- pin 12 to 10 kOhm potentiometer (headphones volume control)
- pin 10 to button 1
- pin 24 to button 2
- pin 30 to buzzer


