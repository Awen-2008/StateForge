#include "StateForge.h"

// Define button pin, currently using GPIO9 to connect button (to GND when pressed)
#define BUTTON_PIN 9

// Define LED pin
#define LED_PIN 6  // Use onboard LED, or specify other pin

// Create button state machine instance
ButtonStateMachine button;

// Variables for non-blocking delay
unsigned long lastLoopTime = 0;         // Last call time
const unsigned long loopInterval = 10;  // Loop interval time (10ms)

// Variables for LED blinking
unsigned long lastBlinkTime = 0;          // Last LED blink time
bool isBlinking = false;                  // LED blinking state
int blinkCount = 0;                       // LED blink counter
const unsigned long blinkInterval = 500;  // LED blink interval (500ms)

/*
 * LED blinking logic explanation:
 * 1. When a double click event is detected, set isBlinking to true to start blinking
 * 2. In the loop function, if isBlinking is true, toggle LED state every 500ms
 * 3. After blinking 5 times (10 state changes), automatically stop blinking and turn off LED
 * 4. During blinking, the button state machine can still detect other events
 */

void setup() {
  // 初始化串口，用于调试输出
  Serial.begin(115200);
  while (!Serial) {
    ;  // 等待串口连接
  }

  // 初始化LED引脚为输出模式
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // 初始关闭LED

  // 初始化按钮，设置长按时间为1000ms，双击间隔时间为400ms
  button.init(BUTTON_PIN, 1000, 400);     // 当前按键电平配置使用了上拉电阻，所以其激活电平设置为默认值。

  Serial.println("Button state machine initialized");
  Serial.println("Using non-blocking delay method");
  Serial.println("Button connection: one end to GPIO9, other end to GND");
  Serial.println("Initial state: button not pressed (pin is high)");
  Serial.println("Operation instructions:");
  Serial.println("- Single click: Turn on LED");
  Serial.println("- Double click: LED blinks at 500ms intervals");
  Serial.println("- Long press: Turn off LED");
}

void loop() {
  // 使用非阻塞延时方式
  if (checkDelay(loopInterval, lastLoopTime)) {  // 每10ms执行一次循环体内的代码
    // 读取原始按钮状态用于调试
    int rawButtonState = digitalRead(BUTTON_PIN);
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > 2000) {  // 每2秒打印一次原始按钮状态
      Serial.print("Current button state: ");
      Serial.println(rawButtonState == HIGH ? "        High level (not pressed)" : "        Low level (pressed)");
      lastDebugTime = millis();
    }

    // 获取按键事件（使用getLastEvent()而非getEvent()）
    // getLastEvent()会在返回事件后自动标记为已处理，避免重复处理
    button_event_t event = button.getLastEvent();

    // 处理按钮事件
    switch (event) {
      case BUTTON_EVENT_SHORT_PRESS:
        // Short press - Turn on LED
        Serial.println("Current button state: Short press - Turn on LED");
        digitalWrite(LED_PIN, HIGH);
        // Event automatically handled by library
        break;

      case BUTTON_EVENT_LONG_PRESS:
        // Long press - Turn off LED
        Serial.println("Current button state: Long press - Turn off LED");
        digitalWrite(LED_PIN, LOW);
        // Event automatically handled by library
        break;

      case BUTTON_EVENT_DOUBLE_PRESS:
        // Double click - LED blinks at 500ms (non-blocking)
        Serial.println("Current button state: Double click - LED blinks at 500ms");
        isBlinking = true;
        lastBlinkTime = millis();
        blinkCount = 0;  // Reset blink counter
        // Event automatically handled by library
        break;

      case BUTTON_EVENT_NONE:
        // 没有事件，不需要处理
        break;
    }

    // 如果正在闪烁LED，处理闪烁逻辑
    if (isBlinking) {
      if (checkDelay(blinkInterval, lastBlinkTime)) {
        // 切换LED状态
        static bool ledState = false;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);

        // 检查闪烁是否完成（5次闪烁共10次状态变化）
        blinkCount++;
        if (blinkCount >= 10) {
          isBlinking = false;
          blinkCount = 0;
          digitalWrite(LED_PIN, LOW);  // 确保LED最终关闭
        }
      }
    }
  }
  // 非阻塞延时已在函数开头处理
}
