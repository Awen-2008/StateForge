#ifndef _STATEFORGE_H
#define _STATEFORGE_H

#include <Arduino.h>

/**
 * @brief Non-blocking delay check
 * @param delayMs Delay time in milliseconds
 * @param lastTime Reference to the last call time
 * @return Returns true if the delay time has elapsed, otherwise false
 */
inline bool checkDelay(unsigned long delayMs, unsigned long &lastTime) {
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= delayMs) {
    lastTime = currentTime;
    return true;
  }

  return false;
}
//----------------------------------------------------------------------------

// Button event types
typedef enum {
  BUTTON_EVENT_NONE,         // No event
  BUTTON_EVENT_SHORT_PRESS,  // Short press
  BUTTON_EVENT_LONG_PRESS,   // Long press
  BUTTON_EVENT_DOUBLE_PRESS  // Double press
} button_event_t;

// Button state types
typedef enum {
  BUTTON_STATE_IDLE,         // Idle state
  BUTTON_STATE_PRESS,        // Pressed state
  BUTTON_STATE_RELEASE,      // Released state
  BUTTON_STATE_DOUBLE_CLICK  // Double click state
} button_state_t;

/**
 * @brief Button state machine class
 *
 * This class implements a button state machine that can detect short press, 
 * long press and double click events.
 * Usage:
 * 1. Create a ButtonStateMachine object
 * 2. Call the init() method to initialize the button pin
 * 3. Call the getEvent() method in the loop to get button events
 * 4. Perform appropriate actions based on the returned event type
 */
class ButtonStateMachine {
public:
  // 构造函数
  ButtonStateMachine();

  /**
     * @brief Initialize the button
     * @param pin Button pin number
     * @param longPressTime Long press detection time in milliseconds, default 1000ms
     * @param doubleClickTime Double click detection time in milliseconds, default 400ms
     * @param activeLow Active level, default true
     */
  void init(uint8_t pin, unsigned long longPressTime = 1000, unsigned long doubleClickTime = 400, bool activeLow = true);

  /**
     * @brief Get button event
     * @return Button event type
     */
  button_event_t getEvent();

  /**
     * @brief Detect button click event (polling method)
     * @return Returns true if a click is detected, otherwise false
     */
  bool isClicked();

  /**
   * @brief Get the last detected button event
   * @return Button event type
   */
  button_event_t getLastEvent();

private:
  uint8_t _pin;                       // 按钮引脚
  unsigned long _longPressTime;       // 长按判定时间
  unsigned long _doubleClickTime;     // 双击判定时间
  bool _activeLow;                    // 按键是否低电平有效

  button_state_t _buttonState;        // 按钮状态
  unsigned long _pressTime;           // 按下时间
  unsigned long _releaseTime;         // 释放时间
  button_event_t _lastEvent;          // 上一个事件
  bool _lastPressed;                  // 上一次按钮状态
  bool _eventProcessed;               // 事件是否已处理
};

#endif  // _STATEFORGE_H
