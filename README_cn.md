# StateForge - Arduino 按键状态机库

StateForge 是一个用于 Arduino 的非阻塞式按键状态机库，可以检测短按、长按和双击事件。

## 特性

- 非阻塞式设计，不会影响其他任务的执行
- 支持短按、长按和双击事件检测
- 支持低电平有效和高电平有效的按键配置
- 内置消抖功能
- 提供非阻塞延时功能
- 简单易用的 API

## 安装

### 方法一：通过 Arduino IDE 安装

1. 打开 Arduino IDE
2. 转到 "工具" > "管理库..."
3. 搜索 "StateForge"
4. 点击安装

### 方法二：手动安装

1. 下载此库的最新版本
2. 解压下载的文件
3. 将 StateForge 文件夹复制到 Arduino 的 libraries 目录下
   - Windows: `Documents\Arduino\libraries\`
   - macOS: `Documents/Arduino/libraries/`
   - Linux: `Arduino/libraries/` (通常在用户主目录下)

## 使用方法

### 基本使用

```cpp
#include "StateForge.h"

// 定义按钮引脚
#define BUTTON_PIN 9

// 创建按键状态机对象
ButtonStateMachine button;

void setup() {
  // 初始化串口
  Serial.begin(115200);

  // 初始化按钮，设置长按时间为1000ms，双击时间为400ms
  // 默认低电平有效（按下时为LOW），使用INPUT_PULLUP模式（内部上拉电阻）
  button.init(BUTTON_PIN, 1000, 400);

  Serial.println("按键状态机初始化完成");
}

void loop() {
  // 获取按钮事件（使用getLastEvent()而非getEvent()）
  // getLastEvent()会在返回事件后自动标记为已处理，避免重复处理
  button_event_t event = button.getLastEvent();

  // 处理按钮事件
  switch (event) {
    case BUTTON_EVENT_SHORT_PRESS:
      // 短按
      Serial.println("短按事件");
      break;

    case BUTTON_EVENT_LONG_PRESS:
      // 长按
      Serial.println("长按事件");
      break;

    case BUTTON_EVENT_DOUBLE_PRESS:
      // 双击
      Serial.println("双击事件");
      break;

    case BUTTON_EVENT_NONE:
      // 没有事件
      break;
  }

  // 短暂延时（可选）
  delay(10);
}
```

### 按键电平配置

默认情况下，按键是低电平有效的（按下时为LOW）。如果您的按键是高电平有效的（按下时为HIGH），可以在初始化时指定：

```cpp
// 低电平有效（默认）
button.init(BUTTON_PIN, 1000, 400, true);

// 高电平有效
button.init(BUTTON_PIN, 1000, 400, false);
```

注意：库会根据activeLow参数自动选择合适的输入模式：
- 低电平有效（activeLow=true）：使用INPUT_PULLUP模式（内部上拉电阻）
- 高电平有效（activeLow=false）：使用INPUT模式，需要外部下拉电阻（10kΩ电阻连接到GND）

对于高电平有效的按键，建议使用外部下拉电阻以确保可靠的检测。

### 非阻塞延时

StateForge 库提供了非阻塞式延时功能，使用 `checkDelay()` 静态方法，不会阻塞程序执行。

```cpp
// 定义非阻塞延时的变量
unsigned long lastLoopTime = 0;
const unsigned long loopInterval = 10;  // 每次循环间隔的时间 (10ms)

void loop() {
  // 使用非阻塞延时方式
  if (checkDelay(loopInterval, lastLoopTime)) {
    // 每10ms执行一次循环体内的代码
    // 在这里处理按钮事件和其他任务
  }
}
```

推荐使用非阻塞延时方式，特别是在需要同时处理多个任务的场景中。

## API 参考

### ButtonStateMachine 类

#### 构造函数
```cpp
ButtonStateMachine();
```

#### init()
```cpp
void init(uint8_t pin, unsigned long longPressTime = 1000, unsigned long doubleClickTime = 400, bool activeLow = true);
```
- **pin**: 按钮连接的引脚
- **longPressTime**: 长按判定时间（毫秒），默认为1000ms
- **doubleClickTime**: 双击判定时间（毫秒），默认为400ms
- **activeLow**: 激活电平，默认为 true（低电平有效）

#### getEvent()
```cpp
button_event_t getEvent();
```
获取按钮事件，不会自动标记事件为已处理。适合需要连续检测事件的场景。

#### getLastEvent()
```cpp
button_event_t getLastEvent();
```
获取按钮事件，并在返回后自动标记事件为已处理。推荐使用此方法以避免重复处理相同事件。

#### isClicked()
```cpp
bool isClicked();
```
检测按钮单击事件（轮询方式），如果检测到单击事件返回true，否则返回false。

### 事件类型

```cpp
typedef enum {
  BUTTON_EVENT_NONE,         // 无事件
  BUTTON_EVENT_SHORT_PRESS,  // 短按
  BUTTON_EVENT_LONG_PRESS,   // 长按
  BUTTON_EVENT_DOUBLE_PRESS  // 双击
} button_event_t;
```

### 状态类型

```cpp
typedef enum {
  BUTTON_STATE_IDLE,         // 空闲状态
  BUTTON_STATE_PRESS,        // 按下状态
  BUTTON_STATE_RELEASE,      // 释放状态
  BUTTON_STATE_DOUBLE_CLICK  // 双击状态
} button_state_t;
```

### 非阻塞延时函数

```cpp
inline bool checkDelay(unsigned long delayMs, unsigned long &lastTime);
```
- **delayMs**: 需要延时的时间（毫秒）
- **lastTime**: 上一次调用的时间（通过引用传递）
- 返回值：如果延时时间已到则返回true，否则返回false

## 示例

请查看 `examples` 目录中的示例代码，了解更多使用方法。

## 注意事项

1. 推荐使用 `getLastEvent()` 而不是 `getEvent()`，因为 `getLastEvent()` 会自动标记事件为已处理，避免重复处理相同事件
2. 按钮应连接到指定引脚和GND之间，使用内部上拉电阻（低电平有效）或外部下拉电阻（高电平有效）
3. 对于高电平有效的按键，建议使用外部下拉电阻（10kΩ电阻连接到GND）以确保可靠的检测
4. 使用非阻塞延时方式，特别是在需要同时处理多个任务的场景中

## 贡献

欢迎提交问题和功能请求！如果您想为此项目做出贡献，请遵循以下步骤：

1. Fork 此仓库
2. 创建您的特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交您的更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开一个 Pull Request

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 作者

- Awen - 初始作者 - [您的GitHub链接]

## 致谢

- 感谢所有为此项目做出贡献的人