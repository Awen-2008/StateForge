/*
 * ESP32 FreeRTOS Button LED Control Example
 * 本示例是基于ESP32 + FreeRTOS环境，使用StateForge库实现的多任务按键控制LED功能。
 * 使用一个按键控制一个LED灯，实现短按点亮LED，双击LED闪烁，长按LED熄灭的效果。
 */

// 调试模式开关，1:开启调试输出，0:关闭调试输出
#define DEBUG 0

// 调试打印宏，只在DEBUG为1时才执行打印
#if DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#include "StateForge.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#define BUTTON_PIN 26             // 定义按键引脚
#define LED_PIN    12             // 定义LED引脚
#define BLINKCOUNT 3              // LED 灯闪烁次数

// 创建按键状态机实例
ButtonStateMachine button;

// 任务句柄
TaskHandle_t buttonTaskHandle;
TaskHandle_t ledTaskHandle;

// 队列句柄
QueueHandle_t eventQueue;

// 按键事件枚举
enum ButtonEvent {
  EVENT_NONE,
  EVENT_SHORT_PRESS,
  EVENT_LONG_PRESS,
  EVENT_DOUBLE_PRESS
};

// 变量用于LED闪烁
unsigned long lastBlinkTime = 0;
bool isBlinking = false;
int blinkCount = 0;
const unsigned long blinkInterval = 500;                // LED闪烁间隔(500ms)。


// 按键任务
void buttonTask(void *pvParameters) {
  unsigned long lastLoopTime = 0;
  const unsigned long loopInterval = 5;                 // 按键检测每 5ms 循环一次，提高响应性
  Serial.println("首次进入【按键任务】");
  for (;;) {
    if (checkDelay(loopInterval, lastLoopTime)) {
      // 获取按键事件
      button_event_t event = button.getLastEvent();
      
      // 添加调试输出
      static unsigned long lastDebugTime = 0;
      int rawButtonState = digitalRead(BUTTON_PIN);
      
      if (millis() - lastDebugTime > 2000) {  // 每2秒打印一次原始按钮状态
        DEBUG_PRINT("原始按钮状态: ");
        DEBUG_PRINTLN(rawButtonState == HIGH ? "高电平(未按下)" : "低电平(按下)");
        lastDebugTime = millis();
      }

      // 将事件发送到队列
      ButtonEvent btnEvent;
      switch (event) {
        case BUTTON_EVENT_SHORT_PRESS:
          DEBUG_PRINTLN("检测到短按事件");
          btnEvent = EVENT_SHORT_PRESS;
          xQueueSend(eventQueue, (void *)&btnEvent, 0);
          break;

        case BUTTON_EVENT_LONG_PRESS:
          DEBUG_PRINTLN("检测到长按事件");
          btnEvent = EVENT_LONG_PRESS;
          xQueueSend(eventQueue, (void *)&btnEvent, 0);
          break;

        case BUTTON_EVENT_DOUBLE_PRESS:
          DEBUG_PRINTLN("检测到双击事件");
          btnEvent = EVENT_DOUBLE_PRESS;
          xQueueSend(eventQueue, (void *)&btnEvent, 0);
          break;

        case BUTTON_EVENT_NONE:
          // 没有事件，不需要处理
          break;
      }
    }

    // 任务延时5ms，提高响应性
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

// LED控制任务
void ledTask(void *pvParameters) {
  Serial.println("首次进入【LED 任务】");
  for (;;) {
    // 从队列获取事件，但不阻塞
    ButtonEvent event;
    if(xQueueReceive(eventQueue, (void *)&event, 0)) {
      switch (event) {
        case EVENT_SHORT_PRESS:
          // 短按 - 点亮LED
          DEBUG_PRINTLN("按键事件: 短按 - 点亮 LED");
          digitalWrite(LED_PIN, HIGH);
          isBlinking = false;  // 停止闪烁
          break;

        case EVENT_LONG_PRESS:
          // 长按 - 关闭LED
          DEBUG_PRINTLN("按键事件: 长按 - 关闭 LED");
          digitalWrite(LED_PIN, LOW);
          isBlinking = false;  // 停止闪烁
          break;

        case EVENT_DOUBLE_PRESS:
          // 双击 - LED闪烁
          DEBUG_PRINTLN("按键事件: 双击 - LED 闪烁");
          if (!isBlinking) {  // 只有在当前没有闪烁时才启动新的闪烁
            digitalWrite(LED_PIN, LOW);  // 确保LED初始状态为关闭
            isBlinking = true;
            lastBlinkTime = millis();
            blinkCount = 0;  // 重置闪烁计数器
            DEBUG_PRINTLN("    LED 开启闪烁(将闪烁5次)。");
          }
          break;

        case EVENT_NONE:
          // 没有事件，不需要处理
          break;
      }
    }

    // 处理LED闪烁逻辑
    if (isBlinking) {
      if (checkDelay(blinkInterval, lastBlinkTime)) {
        // 切换LED状态
        static bool ledState = false;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
        
        // 添加闪烁状态调试信息
        DEBUG_PRINT("LED 闪烁状态: ");
        DEBUG_PRINTLN(ledState ? "ON" : "OFF");
        DEBUG_PRINT("闪烁计数: ");
        DEBUG_PRINTLN(blinkCount);
        lastBlinkTime = millis();

        // 检查闪烁是否完成（5次闪烁共10次状态变化）
        if (blinkCount >= BLINKCOUNT * 2) {
          isBlinking = false;
          blinkCount = 0;
          digitalWrite(LED_PIN, LOW);  // 确保LED最终关闭
          DEBUG_PRINTLN("  LED 闪烁已完成");
        }
        blinkCount++;
      }
    }

    // 任务延时5ms，提高响应性
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void setup() {
  // 初始化串口，用于调试输出
  Serial.begin(115200);
  while (!Serial) {
    ;  // 等待串口连接
  }

  // 初始化LED引脚为输出模式
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // 初始关闭LED

  // 设置按键引脚模式为输入，启用内部上拉电阻(这里的pinMode(BUTTON_PIN, INPUT_PULLUP);在库文件内部已有了，所以注释掉)
  //pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // 初始化按键，设置长按时间为1000ms，双击间隔时间为400ms
  // 使用内部上拉电阻配置，按键另一端接地
  button.init(BUTTON_PIN, 1000, 400);
  
  
  Serial.println(" 程序初始化已完成！");

  // 添加调试信息
  DEBUG_PRINT("按键初始化已完成，引脚: ");
  DEBUG_PRINTLN(BUTTON_PIN);
  DEBUG_PRINT("内部上拉电阻: ");
  DEBUG_PRINTLN(true ? "启用" : "禁用");
  DEBUG_PRINT("长按时间: 1000ms, 双击间隔: 400ms");
  DEBUG_PRINTLN("\n按键接线方式: GPIO26 -> 按键 -> GND");
  DEBUG_PRINTLN("初始状态: 按键未按下(引脚为高电平)");
  DEBUG_PRINTLN("按下按键时: 引脚变为低电平");

  // 创建事件队列
  eventQueue = xQueueCreate(10, sizeof(ButtonEvent));
  if (eventQueue == 0) {
    DEBUG_PRINTLN("错误: 无法创建事件队列");
    while(1);
  }

  // 创建按键任务
  xTaskCreate(
    buttonTask,          // 任务函数
    "ButtonTask",        // 任务名称
    2048,                // 堆栈大小
    NULL,                // 参数
    1,                   // 优先级
    &buttonTaskHandle    // 任务句柄
  );

  // 创建LED控制任务
  xTaskCreate(
    ledTask,             // 任务函数
    "LEDTask",           // 任务名称
    2048,                // 堆栈大小
    NULL,                // 参数
    1,                   // 优先级
    &ledTaskHandle       // 任务句柄
  );

  DEBUG_PRINTLN("ESP32 FreeRTOS 按键LED控制示例程序初始化完成");
  DEBUG_PRINTLN("按键连接方式：一端连接到GPIO26，另一端接地");
  DEBUG_PRINTLN("初始状态：按键未按下(引脚为高电平)");
  DEBUG_PRINTLN("操作说明:");
  DEBUG_PRINTLN("- 单击按键: 点亮LED");
  DEBUG_PRINTLN("- 双击按键: LED闪烁5次");
  DEBUG_PRINTLN("- 长按按键: 关闭LED");
  DEBUG_PRINTLN("系统已启动，正在使用FreeRTOS多任务处理按键和LED控制");
}

void loop() {
  // 主循环保持空闲，所有任务都在FreeRTOS中运行
  //vTaskDelay(pdMS_TO_TICKS(100));
}
