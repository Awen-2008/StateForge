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

## ESP32_FreeRTOS_ButtonLED Example

The ESP32_FreeRTOS_ButtonLED example demonstrates how to use the StateForge library with ESP32 and FreeRTOS to implement multi-task button control of an LED. This example uses FreeRTOS tasks to handle button events and LED control separately.

### Hardware Connections
- Button: One end connected to ESP32 GPIO26 pin, the other end to GND
- LED: Connected to ESP32 GPIO2 pin (built-in LED)

### Function Description
- **Short Press**: Turn on the LED
- **Double Click**: LED blinks 5 times at 500ms intervals
- **Long Press**: Turn off the LED

### FreeRTOS Task Architecture
- **Button Task**: Monitors button presses and sends events to a queue
- **LED Task**: Receives events from the queue and controls the LED accordingly
- **Event Queue**: Facilitates communication between tasks

### Usage Instructions
1. Connect the hardware as described
2. Open ESP32_FreeRTOS_ButtonLED.ino in the Arduino IDE
3. Select the appropriate ESP32 board in the Arduino IDE
4. Upload the code to the ESP32 board
5. Open the Serial Monitor (baud rate set to 115200) to view debug information
6. Test different button operations

### Code Description
This example demonstrates multi-task programming with FreeRTOS:
- Two independent tasks handle button monitoring and LED control
- Events are passed between tasks via a FreeRTOS queue
- The main loop remains idle as all functionality is handled by tasks
- Non-blocking delays ensure responsive button detection even during LED blinking
