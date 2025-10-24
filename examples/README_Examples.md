# StateForge Examples

This directory contains usage examples for the StateForge library.

## ButtonTest Example

The ButtonTest example demonstrates how to use the StateForge library to detect button short press, long press, and double click events, with feedback through an LED.

### Hardware Connections
- Button: One end connected to Arduino GPIO9 pin, the other end to GND
- LED: Connected to Arduino GPIO6 pin (or modify the code to use another pin)

### Function Description
- **Short Press**: Turn on the LED
- **Double Click**: LED blinks 5 times at 500ms intervals
- **Long Press**: Turn off the LED

### Usage Instructions
1. Connect the hardware as described
2. Open ButtonTest.ino in the Arduino IDE
3. Upload the code to the Arduino board
4. Open the Serial Monitor (baud rate set to 115200) to view debug information
5. Test different button operations

### Code Description
The example code uses a non-blocking delay method to ensure that the button state machine continues to work properly while the LED is blinking.
