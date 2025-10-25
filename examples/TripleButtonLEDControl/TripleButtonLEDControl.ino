#include "StateForge.h"

// 定义按键引脚
#define BUTTON1_PIN 26
#define BUTTON2_PIN 25
#define BUTTON3_PIN 33

// 定义LED引脚
#define LED1_PIN 12
#define LED2_PIN 14
#define LED3_PIN 27

// 创建三个按键状态机实例
ButtonStateMachine button1;
ButtonStateMachine button2;
ButtonStateMachine button3;

// 变量用于非阻塞延时
unsigned long lastLoopTime = 0;         // 上一次循环调用时间
const unsigned long loopInterval = 10;  // 循环间隔时间(10ms)

// 变量用于LED闪烁
unsigned long lastBlinkTime1 = 0;       // LED1最后闪烁时间
unsigned long lastBlinkTime2 = 0;       // LED2最后闪烁时间
unsigned long lastBlinkTime3 = 0;       // LED3最后闪烁时间
bool isBlinking1 = false;               // LED1闪烁状态
bool isBlinking2 = false;               // LED2闪烁状态
bool isBlinking3 = false;               // LED3闪烁状态
int blinkCount1 = 0;                    // LED1闪烁计数器
int blinkCount2 = 0;                    // LED2闪烁计数器
int blinkCount3 = 0;                    // LED3闪烁计数器
const unsigned long blinkInterval = 500; // LED闪烁间隔(500ms)

void setup() {
  // 初始化串口，用于调试输出
  Serial.begin(115200);
  while (!Serial) {
    ;  // 等待串口连接
  }

  // 初始化LED引脚为输出模式
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);  // 初始关闭所有LED
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);

  // 初始化按键，设置长按时间为1000ms，双击间隔时间为400ms
  // 使用内部上拉电阻，按键另一端接地
  button1.init(BUTTON1_PIN, 1000, 400, true);
  button2.init(BUTTON2_PIN, 1000, 400, true);
  button3.init(BUTTON3_PIN, 1000, 400, true);

  Serial.println("三键控制三灯状态机示例程序初始化完成");
  Serial.println("按键连接方式：一端连接到指定GPIO，另一端接地");
  Serial.println("初始状态：按键未按下(引脚为高电平)");
  Serial.println("操作说明:");
  Serial.println("- 单击按键: 点亮对应的LED");
  Serial.println("- 双击按键: 对应的LED闪烁");
  Serial.println("- 长按按键: 关闭对应的LED");
}

void loop() {
  // 使用非阻塞延时方式
  if (checkDelay(loopInterval, lastLoopTime)) {  // 每10ms执行一次循环体内的代码

    // 获取按键事件（使用getLastEvent()而非getEvent()）
    // getLastEvent()会在返回事件后自动标记为已处理，避免重复处理
    button_event_t event1 = button1.getLastEvent();
    button_event_t event2 = button2.getLastEvent();
    button_event_t event3 = button3.getLastEvent();

    // 处理按键1事件
    switch (event1) {
      case BUTTON_EVENT_SHORT_PRESS:
        // 单击 - 点亮LED1
        Serial.println("按键1: 单击 - 点亮LED1");
        digitalWrite(LED1_PIN, HIGH);
        isBlinking1 = false;  // 停止闪烁
        break;

      case BUTTON_EVENT_LONG_PRESS:
        // 长按 - 关闭LED1
        Serial.println("按键1: 长按 - 关闭LED1");
        digitalWrite(LED1_PIN, LOW);
        isBlinking1 = false;  // 停止闪烁
        break;

      case BUTTON_EVENT_DOUBLE_PRESS:
        // 双击 - LED1闪烁
        Serial.println("按键1: 双击 - LED1闪烁");
        isBlinking1 = true;
        lastBlinkTime1 = millis();
        blinkCount1 = 0;  // 重置闪烁计数器
        break;

      case BUTTON_EVENT_NONE:
        // 没有事件，不需要处理
        break;
    }

    // 处理按键2事件
    switch (event2) {
      case BUTTON_EVENT_SHORT_PRESS:
        // 单击 - 点亮LED2
        Serial.println("按键2: 单击 - 点亮LED2");
        digitalWrite(LED2_PIN, HIGH);
        isBlinking2 = false;  // 停止闪烁
        break;

      case BUTTON_EVENT_LONG_PRESS:
        // 长按 - 关闭LED2
        Serial.println("按键2: 长按 - 关闭LED2");
        digitalWrite(LED2_PIN, LOW);
        isBlinking2 = false;  // 停止闪烁
        break;

      case BUTTON_EVENT_DOUBLE_PRESS:
        // 双击 - LED2闪烁
        Serial.println("按键2: 双击 - LED2闪烁");
        isBlinking2 = true;
        lastBlinkTime2 = millis();
        blinkCount2 = 0;  // 重置闪烁计数器
        break;

      case BUTTON_EVENT_NONE:
        // 没有事件，不需要处理
        break;
    }

    // 处理按键3事件
    switch (event3) {
      case BUTTON_EVENT_SHORT_PRESS:
        // 单击 - 点亮LED3
        Serial.println("按键3: 单击 - 点亮LED3");
        digitalWrite(LED3_PIN, HIGH);
        isBlinking3 = false;  // 停止闪烁
        break;

      case BUTTON_EVENT_LONG_PRESS:
        // 长按 - 关闭LED3
        Serial.println("按键3: 长按 - 关闭LED3");
        digitalWrite(LED3_PIN, LOW);
        isBlinking3 = false;  // 停止闪烁
        break;

      case BUTTON_EVENT_DOUBLE_PRESS:
        // 双击 - LED3闪烁
        Serial.println("按键3: 双击 - LED3闪烁");
        isBlinking3 = true;
        lastBlinkTime3 = millis();
        blinkCount3 = 0;  // 重置闪烁计数器
        break;

      case BUTTON_EVENT_NONE:
        // 没有事件，不需要处理
        break;
    }

    // 如果正在闪烁LED，处理闪烁逻辑
    if (isBlinking1) {
      if (checkDelay(blinkInterval, lastBlinkTime1)) {
        // 切换LED1状态
        static bool led1State = false;
        led1State = !led1State;
        digitalWrite(LED1_PIN, led1State ? HIGH : LOW);

        // 检查闪烁是否完成（5次闪烁共10次状态变化）
        blinkCount1++;
        if (blinkCount1 >= 10) {
          isBlinking1 = false;
          blinkCount1 = 0;
          digitalWrite(LED1_PIN, LOW);  // 确保LED1最终关闭
        }
      }
    }

    if (isBlinking2) {
      if (checkDelay(blinkInterval, lastBlinkTime2)) {
        // 切换LED2状态
        static bool led2State = false;
        led2State = !led2State;
        digitalWrite(LED2_PIN, led2State ? HIGH : LOW);

        // 检查闪烁是否完成（5次闪烁共10次状态变化）
        blinkCount2++;
        if (blinkCount2 >= 10) {
          isBlinking2 = false;
          blinkCount2 = 0;
          digitalWrite(LED2_PIN, LOW);  // 确保LED2最终关闭
        }
      }
    }

    if (isBlinking3) {
      if (checkDelay(blinkInterval, lastBlinkTime3)) {
        // 切换LED3状态
        static bool led3State = false;
        led3State = !led3State;
        digitalWrite(LED3_PIN, led3State ? HIGH : LOW);

        // 检查闪烁是否完成（5次闪烁共10次状态变化）
        blinkCount3++;
        if (blinkCount3 >= 10) {
          isBlinking3 = false;
          blinkCount3 = 0;
          digitalWrite(LED3_PIN, LOW);  // 确保LED3最终关闭
        }
      }
    }
  }
  // 非阻塞延时已在函数开头处理
}
