# 体感拳皇 (Motion Sensing King of Fighters)

基于 Arduino 和 IMU 传感器的体感控制系统，通过可穿戴设备捕捉玩家动作，将身体动作实时转化为键盘输入，用于控制《拳皇》等格斗游戏。

A motion-sensing control system based on Arduino and IMU sensors. Wearable devices capture player movements and convert them into real-time keyboard inputs for fighting games like *King of Fighters*.

"体感拳皇"是一个创新的体感交互系统，通过四个可穿戴设备（左/右手、左/右腿）捕捉玩家动作，并将动作转化为键盘输入指令来控制游戏角色。系统结合了硬件、人工智能和软件工程，为格斗游戏提供沉浸式体验。

无论在Windows系统还是macOS都能流畅运行。

---

## 项目结构 / Project Structure

```
体感拳皇/
├── LeftHand/                  # 左手 Arduino 代码 / Left hand module
│   ├── LeftHand.ino
│   └── model.h                # TensorFlow Lite 模型 / ML model
├── RightHand/                 # 右手 Arduino 代码 / Right hand module
│   ├── RightHand.ino
│   └── model.h
├── LeftLeg/                   # 左腿 Arduino 代码 / Left leg module
│   ├── LeftLeg.ino
│   └── model.h
├── RightLeg/                  # 右腿 Arduino 代码 / Right leg module
│   ├── RightLeg.ino
│   └── model.h
├── python数字键盘和蓝牙信号接收/   # PC 端 BLE 控制脚本 / PC-side BLE controller
│   └── PythonProject6/
│       └── SQL.py             # 主控程序 / Main controller
├── python蓝牙地址捕捉/            # BLE 扫描工具 / BLE scanner utility
│   └── PyCharmMiscProject/
│       └── script.py
├── 演示视频/                    # 演示视频 / Demo video
├── requirements.txt            # Python 依赖 / Python dependencies
├── .gitignore
└── README.md
```

---

## 硬件架构 / Hardware Architecture

### 四个可穿戴模块 / Four Wearable Modules

| 模块 / Module | BLE 名称 / Name | 控制功能 / Function |
|:---:|:---:|:---:|
| LeftHand | LeftHand | 前进/后退 / Forward/Backward |
| RightHand | RightHand | 轻拳/重拳 / Light/Heavy Punch |
| LeftLeg | LeftLeg | 跳跃/跳跃 / Jump |
| RightLeg | RightLeg | 蹲下/重踢 / Crouch/Heavy Kick |

### 核心元件 / Core Components

- **主控芯片**：Arduino 开发板（兼容 ArduinoBLE 库）
- **IMU 传感器**：LSM6DS3（3 轴加速度计 + 3 轴陀螺仪，I2C 接口）
- **通信方式**：BLE (Bluetooth Low Energy) 4.0+
- **板载 AI**：TensorFlow Lite Micro（本地边缘推理，降低延迟）

---

## 软件架构 / Software Architecture

### Arduino 端 (C++)

每个模块独立运行，采用**状态机**架构：

```
STATE_IDLE → [检测到加速度超过阈值] → STATE_COLLECTING → STATE_PROCESSING → STATE_IDLE
```

- **STATE_IDLE**：监测 IMU 加速度，使用移动平均滤波等待动作触发
- **STATE_COLLECTING**：以 100Hz 采样率采集 119 帧 IMU 数据（加速度 + 陀螺仪，共 6 通道）
- **STATE_PROCESSING**：运行 TensorFlow Lite 模型推理，根据置信度通过 BLE 发送动作指令

**BLE 服务 UUID**：`12345678-1234-5678-9ABC-DEF12345678X`（X = 0, A, B, C 区分四个模块）

### PC 端 (Python)

- **SQL.py**：主控程序，使用 `bleak` 库并发连接 4 个 BLE 设备，接收通知后将动作映射为键盘输入
- **script.py**：BLE 扫描工具，用于发现设备 MAC 地址
- 采用 `pyautogui` 模拟键盘按键（keyDown/keyUp）

---

## 动作映射 / Action Mapping

| BLE 指令 / Command | 键盘映射 / Key | 游戏动作 / Game Action | 来源 / Source |
|:---:|:---:|:---:|:---:|
| W | W | 前进 / Forward | LeftHand |
| S | S | 后退 / Backward | LeftHand |
| A | A | 跳跃 / Jump | LeftLeg |
| D | D | 蹲下 / Crouch | RightLeg |
| J | J | 轻拳 / Light Punch | RightHand |
| L | L | 重拳 / Heavy Punch | RightHand |
| K | K | 轻踢 / Light Kick | RightHand |
| U | U | 重踢 / Heavy Kick | RightLeg |

---

## 快速开始 / Quick Start

### 前置要求 / Prerequisites

**硬件 / Hardware：**
- 4× Arduino 开发板（支持 BLE）
- 4× LSM6DS3 IMU 传感器
- 电脑需支持 BLE 蓝牙

**软件 / Software：**
- [Arduino IDE](https://www.arduino.cc/en/software)（含 ArduinoBLE、LSM6DS3、TensorFlowLite 库）
- Python 3.7+
- 拳皇等格斗游戏模拟器（如 WinKawaks）

### 安装步骤 / Installation

1. **克隆仓库 / Clone the repo**
   ```bash
   git clone https://github.com/YOUR_USERNAME/motion-kof.git
   cd motion-kof
   ```

2. **安装 Python 依赖 / Install Python dependencies**
   ```bash
   pip install -r requirements.txt
   ```

3. **烧录 Arduino 代码 / Flash Arduino sketches**
   - 分别将 `LeftHand/LeftHand.ino`、`RightHand/RightHand.ino`、`LeftLeg/LeftLeg.ino`、`RightLeg/RightLeg.ino` 烧录到 4 块 Arduino 板上
   - 注意：每个 `.ino` 依赖同目录下的 `model.h`（TensorFlow Lite 模型文件）

4. **扫描 BLE 设备地址 / Scan BLE addresses**
   ```bash
   python "python蓝牙地址捕捉/PyCharmMiscProject/script.py"
   ```

5. **配置设备地址 / Configure device addresses**
   - 编辑 `python数字键盘和蓝牙信号接收/PythonProject6/SQL.py`
   - 将 `DEVICES` 列表中的 MAC 地址更新为实际扫描到的地址

6. **运行主控程序 / Run the controller**
   ```bash
   python "python数字键盘和蓝牙信号接收/PythonProject6/SQL.py"
   ```

7. **启动游戏 / Launch the game**，开始体感操控！

### 使用说明 / Usage

- 通电后 Arduino 板亮**红光**（等待 BLE 连接）
- 打开 Arduino 串口监视器以激活蓝牙信号发射
- Python 程序成功连接后，**红光熄灭**（见 `演示提醒.txt`）
- 在 Python 控制台输入 `Q` 可安全退出程序
- 系统具备自动重连功能（指数退避，最多 5 次尝试）

---

## 技术亮点 / Technical Highlights

1. **边缘 AI 推理**：TensorFlow Lite Micro 模型在 Arduino 本地运行，无需将传感器数据传到 PC，大幅降低延迟
2. **分布式传感网络**：4 个独立节点分别监测四肢，通过 BLE 并发通信
3. **双重动作识别**：IMU 惯性数据 + TensorFlow 模型，每个模块可识别 2 种不同的手势
4. **鲁棒性设计**：BLE 自动重连（指数退避）、移动平均滤波、动作去抖
5. **低延迟**：100Hz IMU 采样 + 板载推理 + BLE 通知推送，端到端延迟 < 100ms

---

## 贡献者 / Contributors

- [tele-boy](https://github.com/tele-boy)
- [PVn258](https://github.com/PVn258)

## 许可证 / License

[MIT License](LICENSE)

---

## 演示 / Demo

项目包含演示视频（`演示视频/` 目录），展示了系统的实际运行效果。
