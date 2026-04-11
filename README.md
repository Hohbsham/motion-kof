# 体感拳皇 (Motion Sensing King of Fighters)

基于Arduino和IMU传感器的体感控制系统，用于控制《拳皇》等格斗游戏。

## 项目简介

"体感拳皇"是一个创新的体感交互系统，通过四个可穿戴设备（左/右手、左/右腿）捕捉玩家动作，并将动作转化为键盘输入指令来控制游戏角色。系统结合了硬件、人工智能和软件工程，为格斗游戏提供沉浸式体验。

## 硬件架构

### 四个Arduino模块
- **LeftHand**（左手）
- **RightHand**（右手）
- **LeftLeg**（左腿）
- **RightLeg**（右腿）

### 传感器
- LSM6DS3 IMU传感器（用于检测动作和手势）
- 每个模块都内置TensorFlow Lite模型进行本地手势识别

## 软件架构

### Arduino端
- 每个模块运行TensorFlow Lite模型
- 识别手势动作（阈值设置为0.8置信度）
- 通过BLE（蓝牙低功耗）传输识别结果到PC

### PC端Python脚本
- **script.py**：初步的串口通信测试版本
- **SQL.py**：主要控制脚本，负责：
  - 连接四个BLE设备
  - 接收各模块发送的动作指令
  - 将动作映射为键盘按键
  - 自动控制游戏

## 动作映射系统

### 方向控制
- W: 前进
- S: 后退
- A: 左移
- D: 右移

### 攻击控制
- J: 轻拳
- K: 重拳
- L: 轻踢
- U: 重踢

### 特性
- 自动重连：具备BLE连接断线重连功能
- 分布式传感：四个独立的传感器节点分别监测身体不同部位
- 本地AI推理：每个Arduino模块上运行TensorFlow Lite模型，减少延迟
- 实时响应：通过BLE实现低延迟动作传输
- 容错机制：具备连接重试和错误处理机制

## 技术亮点

1. **分布式传感**：四个独立的传感器节点分别监测身体不同部位
2. **本地AI推理**：每个Arduino模块上运行TensorFlow Lite模型，减少延迟
3. **实时响应**：通过BLE实现低延迟动作传输
4. **容错机制**：具备连接重试和错误处理机制

## 系统要求

### 硬件
- 4个Arduino开发板
- 4个LSM6DS3 IMU传感器
- 蓝牙适配器（支持BLE）

### 软件
- Python 3.7+
- PyAutoGUI
- Bleak (BLE库)
- TensorFlow Lite

## 安装与使用

1. 将Arduino代码烧录到各个模块
2. 启动Python控制脚本
3. 穿戴传感器设备
4. 运行《拳皇》或其他兼容的游戏
5. 开始体感游戏体验

## 目录结构

```
体感拳皇/
├── LeftHand/                 # 左手Arduino代码
├── RightHand/                # 右手Arduino代码
├── LeftLeg/                  # 左腿Arduino代码
├── RightLeg/                 # 右腿Arduino代码
├── python附加库和编译后程序/ # Python控制脚本
│   └── PythonProject6/
│       ├── SQL.py           # 主控制脚本
│       └── ...
├── python编程指南/          # 文档和教程
├── 演示视频/                # 操作演示视频
└── README.md                # 项目说明
```

## 贡献者

- [tele-boy](https://github.com/tele-boy)
- [PVn258](https://github.com/PVn258)

## 许可证

MIT License

## 演示

项目包含演示视频，展示了系统的实际运行效果。