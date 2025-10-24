#include "StateForge.h"

// 构造函数
ButtonStateMachine::ButtonStateMachine() {
    // 初始化成员变量
    _pin = 0;
    _longPressTime = 1000;
    _doubleClickTime = 400;
    _buttonState = BUTTON_STATE_IDLE;
    _pressTime = 0;
    _releaseTime = 0;
    _lastEvent = BUTTON_EVENT_NONE;
    _lastPressed = false;
    _eventProcessed = true;
}

/**
 * @brief Initialize the button
 * @param pin Button pin number
 * @param longPressTime Long press detection time in milliseconds, default 1000ms
 * @param doubleClickTime Double click detection time in milliseconds, default 400ms
 * @param activeLow Whether the button is active low (true=active low, false=active high), default true
 */
void ButtonStateMachine::init(uint8_t pin, unsigned long longPressTime, unsigned long doubleClickTime, bool activeLow) {
    _pin = pin;
    _longPressTime = longPressTime;
    _doubleClickTime = doubleClickTime;
    _activeLow = activeLow;

    // Select input mode based on activeLow parameter
    // activeLow=true: Active low, use pull-up resistor (default)
    // activeLow=false: Active high, use external pull-down resistor or internal pull-down (if supported)
    pinMode(_pin, _activeLow ? INPUT_PULLUP : INPUT);
    _buttonState = BUTTON_STATE_IDLE;
    _pressTime = 0;
    _releaseTime = 0;
    _lastEvent = BUTTON_EVENT_NONE;
    _lastPressed = false;
    _eventProcessed = true;
}

/**
 * @brief Get button event
 * @return Button event type
 */
button_event_t ButtonStateMachine::getEvent() {
    // Determine button state based on activeLow parameter
    bool currentPressed = _activeLow ? (digitalRead(_pin) == LOW) : (digitalRead(_pin) == HIGH);
    unsigned long currentTime = millis();

    // If the previous event hasn't been processed, continue returning that event
    if (!_eventProcessed) {
        return _lastEvent;
    }

    // State machine logic
    switch (_buttonState) {
    case BUTTON_STATE_IDLE:
        if (currentPressed && !_lastPressed) {
            // Button transitioned from released to pressed state
            _pressTime = currentTime;
            _buttonState = BUTTON_STATE_PRESS;
        }
        break;

    case BUTTON_STATE_PRESS:
        if (!currentPressed && _lastPressed) {
            // Button transitioned from pressed to released state
            _releaseTime = currentTime;

            // Check if it's a long press or possibly a short press/double click
            if (_releaseTime - _pressTime >= _longPressTime) {
                // Long press event
                _lastEvent = BUTTON_EVENT_LONG_PRESS;
                _eventProcessed = false;
                _buttonState = BUTTON_STATE_IDLE; // Return to IDLE state after long press
            } else {
                // Could be a short press or first click of a double click
                _buttonState = BUTTON_STATE_RELEASE;
            }
        } else if (currentPressed && (currentTime - _pressTime >= _longPressTime)) {
            // Long press event (held but reached time threshold without release)
            _lastEvent = BUTTON_EVENT_LONG_PRESS;
            _eventProcessed = false;
            _buttonState = BUTTON_STATE_IDLE;
        }
        break;

    case BUTTON_STATE_RELEASE:
        if (currentPressed && !_lastPressed) {
            // Could be the second press of a double click
            if (currentTime - _releaseTime <= _doubleClickTime) {
                _buttonState = BUTTON_STATE_DOUBLE_CLICK;
            }
        } else if (currentTime - _releaseTime > _doubleClickTime) {
            // Exceeded double click time window, confirm as short press
            _lastEvent = BUTTON_EVENT_SHORT_PRESS;
            _eventProcessed = false;
            _buttonState = BUTTON_STATE_IDLE;
        }
        break;

    case BUTTON_STATE_DOUBLE_CLICK:
        if (!currentPressed && _lastPressed) {
            // Second release of double click, confirm as double click
            _lastEvent = BUTTON_EVENT_DOUBLE_PRESS;
            _eventProcessed = false;
            _buttonState = BUTTON_STATE_IDLE;
        }
        break;
    }

    // Update last button state
    _lastPressed = currentPressed;

    // Return event
    if (!_eventProcessed) {
        return _lastEvent;
    }

    return BUTTON_EVENT_NONE;
}

/**
 * @brief Detect button click event (polling method)
 * @return Returns true if a click is detected, otherwise false
 */
bool ButtonStateMachine::isClicked() {
    static bool lastState = false;  // Record last button state
    static unsigned long lastChangeTime = 0;  // Last state change time

    // Get current button state
    // Determine button state based on activeLow parameter
    bool currentState = _activeLow ? (digitalRead(_pin) == LOW) : (digitalRead(_pin) == HIGH);
    unsigned long now = millis();  // Current time in milliseconds

    // If state changed too quickly (less than debounce time), ignore it
    if (now - lastChangeTime < 20) {
        return false;
    }

    // Detect button press to release process (complete click)
    if (lastState && !currentState) {
        lastState = currentState;
        lastChangeTime = now;
        return true;  // Button just released, return click event
    }

    // Update button state
    if (lastState != currentState) {
        lastState = currentState;
        lastChangeTime = now;
    }

    return false;
}

/**
 * @brief Get the last detected button event
 * @return Button event type
 */
button_event_t ButtonStateMachine::getLastEvent() {
    // Detect button event
    button_event_t event = getEvent();

    // If there's an event, return it and mark as processed
    if (event != BUTTON_EVENT_NONE) {
        _eventProcessed = true;
        return event;
    }

    // If no event, return no event
    return BUTTON_EVENT_NONE;
}
