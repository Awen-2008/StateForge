# StateForge - Arduino Button State Machine Library

StateForge is a non-blocking button state machine library for Arduino that can detect short press, long press, and double click events.

## Features

- Non-blocking design that doesn't interfere with other tasks
- Support for short press, long press, and double click event detection
- Support for both active-low and active-high button configurations
- Built-in debouncing functionality
- Non-blocking delay functionality
- Simple and easy-to-use API

## Installation

### Method 1: Install via Arduino IDE

1. Open Arduino IDE
2. Go to "Tools" > "Manage Libraries..."
3. Search for "StateForge"
4. Click Install

### Method 2: Manual Installation

1. Download the latest version of this library
2. Extract the downloaded file
3. Copy the StateForge folder to the Arduino libraries directory
   - Windows: `Documents\Arduino\libraries\`
   - macOS: `Documents/Arduino/libraries/`
   - Linux: `Arduino/libraries/` (usually in the user's home directory)

## Usage

### Basic Usage

```cpp
#include "StateForge.h"

// Define button pin
#define BUTTON_PIN 9

// Create button state machine object
ButtonStateMachine button;

void setup() {
  // Initialize serial
  Serial.begin(115200);

  // Initialize button with long press time of 1000ms and double click time of 400ms
  // Default is active-low (pressed state is LOW), using INPUT_PULLUP mode (internal pull-up resistor)
  button.init(BUTTON_PIN, 1000, 400);

  Serial.println("Button state machine initialized");
}

void loop() {
  // Get button event (use getLastEvent() instead of getEvent())
  // getLastEvent() automatically marks the event as processed after returning, avoiding repeated processing
  button_event_t event = button.getLastEvent();

  // Handle button events
  switch (event) {
    case BUTTON_EVENT_SHORT_PRESS:
      // Short press
      Serial.println("Short press event");
      break;

    case BUTTON_EVENT_LONG_PRESS:
      // Long press
      Serial.println("Long press event");
      break;

    case BUTTON_EVENT_DOUBLE_PRESS:
      // Double click
      Serial.println("Double click event");
      break;

    case BUTTON_EVENT_NONE:
      // No event
      break;
  }

  // Short delay (optional)
  delay(10);
}
```

### Button Level Configuration

By default, buttons are active-low (pressed state is LOW). If your button is active-high (pressed state is HIGH), you can specify this during initialization:

```cpp
// Active-low (default)
button.init(BUTTON_PIN, 1000, 400, true);

// Active-high
button.init(BUTTON_PIN, 1000, 400, false);
```

Note: The library automatically selects the appropriate input mode based on the activeLow parameter:
- Active-low (activeLow=true): Uses INPUT_PULLUP mode (internal pull-up resistor)
- Active-high (activeLow=false): Uses INPUT mode, requiring an external pull-down resistor (10kΩ resistor to GND)

For active-high buttons, it's recommended to use an external pull-down resistor for reliable detection.

### Non-blocking Delay

The StateForge library provides a non-blocking delay function using the `checkDelay()` static method, which doesn't block program execution.

```cpp
// Define variables for non-blocking delay
unsigned long lastLoopTime = 0;
const unsigned long loopInterval = 10;  // Loop interval time (10ms)

void loop() {
  // Use non-blocking delay method
  if (checkDelay(loopInterval, lastLoopTime)) {
    // Code in this block executes every 10ms
    // Handle button events and other tasks here
  }
}
```

Using non-blocking delay is recommended, especially in scenarios where multiple tasks need to be handled simultaneously.

## API Reference

### ButtonStateMachine Class

#### Constructor
```cpp
ButtonStateMachine();
```

#### init()
```cpp
void init(uint8_t pin, unsigned long longPressTime = 1000, unsigned long doubleClickTime = 400, bool activeLow = true);
```
- **pin**: Button pin number
- **longPressTime**: Long press detection time in milliseconds, default 1000ms
- **doubleClickTime**: Double click detection time in milliseconds, default 400ms
- **activeLow**: Active level, default true (active-low)

#### getEvent()
```cpp
button_event_t getEvent();
```
Get button event, without automatically marking it as processed. Suitable for scenarios where continuous event detection is needed.

#### getLastEvent()
```cpp
button_event_t getLastEvent();
```
Get button event and automatically mark it as processed after returning. Recommended method to avoid processing the same event repeatedly.

#### isClicked()
```cpp
bool isClicked();
```
Detect button click event (polling method), returns true if a click is detected, otherwise false.

### Event Types

```cpp
typedef enum {
  BUTTON_EVENT_NONE,         // No event
  BUTTON_EVENT_SHORT_PRESS,  // Short press
  BUTTON_EVENT_LONG_PRESS,   // Long press
  BUTTON_EVENT_DOUBLE_PRESS  // Double press
} button_event_t;
```

### State Types

```cpp
typedef enum {
  BUTTON_STATE_IDLE,         // Idle state
  BUTTON_STATE_PRESS,        // Pressed state
  BUTTON_STATE_RELEASE,      // Released state
  BUTTON_STATE_DOUBLE_CLICK  // Double click state
} button_state_t;
```

### Non-blocking Delay Function

```cpp
inline bool checkDelay(unsigned long delayMs, unsigned long &lastTime);
```
- **delayMs**: Delay time in milliseconds
- **lastTime**: Reference to the last call time
- Return value: Returns true if the delay time has elapsed, otherwise false

## Examples

Please see the code in the `examples` directory for more usage examples.

## Notes

1. It's recommended to use `getLastEvent()` instead of `getEvent()` because `getLastEvent()` automatically marks the event as processed, avoiding repeated processing of the same event
2. Buttons should be connected between the specified pin and GND, using internal pull-up resistors (for active-low) or external pull-down resistors (for active-high)
3. For active-high buttons, it's recommended to use an external pull-down resistor (10kΩ resistor to GND) for reliable detection
4. Use non-blocking delay methods, especially in scenarios where multiple tasks need to be handled simultaneously

## Contributing

Issues and pull requests are welcome! If you want to contribute to this project, please follow these steps:

1. Fork this repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

## Author

- Awen - Initial author - [Your GitHub link]

## Acknowledgments

- Thanks to all contributors to this project