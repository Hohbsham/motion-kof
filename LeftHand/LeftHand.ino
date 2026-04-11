// #include <ArduinoBLE.h>
// #include <LSM6DS3.h>
// #include <Wire.h>
// #include <TensorFlowLite.h>
// #include <tensorflow/lite/micro/all_ops_resolver.h>
// #include <tensorflow/lite/micro/micro_error_reporter.h>
// #include <tensorflow/lite/micro/micro_interpreter.h>
// #include <tensorflow/lite/schema/schema_generated.h>
// #include "model.h"



// const float accelerationThreshold = 2.5; // 触发阈值（G值）
// const int numSamples = 119;             // 样本数量
// const float confidenceThreshold = 0.8;  // 置信度阈值
// const int S_punch = 2;  // 单按钮：轻拳
// const int leg = 3;  // 轻腿
// // IMU传感器实例
// LSM6DS3 myIMU(I2C_MODE, 0x6A);

// // TensorFlow Lite 配置
// namespace {
// tflite::MicroErrorReporter tflErrorReporter;
// tflite::AllOpsResolver tflOpsResolver;
// const tflite::Model* tflModel = nullptr;
// tflite::MicroInterpreter* tflInterpreter = nullptr;
// TfLiteTensor* tflInputTensor = nullptr;
// TfLiteTensor* tflOutputTensor = nullptr;

// constexpr int tensorArenaSize = 8 * 1024;
// alignas(16) byte tensorArena[tensorArenaSize];
// }  // namespace

// // LED引脚配置
// const int actionLed = LED_BUILTIN;  // 动作检测指示灯

// void setup() {
 
//   pinMode(S_punch, INPUT_PULLUP); 
//   pinMode(leg, INPUT_PULLUP); 
//   Serial.begin(115200);
//   while (!Serial);

//   // 初始化IMU
//   if (myIMU.begin() != 0) {
//     Serial.println("IMU初始化失败");
//     while(1);
//   }
  
//   // 初始化LED
//   pinMode(actionLed, OUTPUT);
//   digitalWrite(actionLed, LOW);

//   // 加载TensorFlow模型
//   tflModel = tflite::GetModel(model);
//   if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
//     Serial.println("模型版本不匹配");
//     while(1);
//   }

//   // 创建解释器
//   tflInterpreter = new tflite::MicroInterpreter(
//     tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);
  


//   // 分配内存
//   if (tflInterpreter->AllocateTensors() != kTfLiteOk) {
//     Serial.println("张量内存分配失败");
//     while(1);
//   }

//   tflInputTensor = tflInterpreter->input(0);
//   tflOutputTensor = tflInterpreter->output(0);

//   Serial.println("系统就绪");
//   Serial.begin(115200);
//   while (!Serial);

//   if (!BLE.begin()) {
//     Serial.println("BLE 初始化失败！");
//     while (1);
//   }


// }

// void loop() {
//   static int samplesRead = numSamples;
//   float aX, aY, aZ, gX, gY, gZ;

//   // 等待有效动作触发
//   if (samplesRead == numSamples) {
//     while (true) {
//       aX = myIMU.readFloatAccelX();
//       aY = myIMU.readFloatAccelY();
//       aZ = myIMU.readFloatAccelZ();
      
//       float accelerationSum = fabs(aX) + fabs(aY) + fabs(aZ);
//       if (accelerationSum >= accelerationThreshold) {
//         samplesRead = 0;
//         break;
//       }
//     }
//   }

//   // 采集数据
//   while (samplesRead < numSamples) {
//     aX = myIMU.readFloatAccelX();
//     aY = myIMU.readFloatAccelY();
//     aZ = myIMU.readFloatAccelZ();
//     gX = myIMU.readFloatGyroX();
//     gY = myIMU.readFloatGyroY();
//     gZ = myIMU.readFloatGyroZ();

//     // 数据标准化（根据实际训练参数调整）
//     tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
//     tflInputTensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
//     tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
//     tflInputTensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
//     tflInputTensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
//     tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

//     samplesRead++;

//     // 完成数据采集时进行预测
//     if (samplesRead == numSamples) {
//       // 执行推理
//       if (tflInterpreter->Invoke() != kTfLiteOk) {
//         Serial.println("推理失败");
//         return;
//       }

//       // 获取预测结果（假设输出层只有一个节点）
//       float confidence = tflOutputTensor->data.f[0];
      
//       // 触发条件判断
//       if (confidence >= confidenceThreshold) {
//         digitalWrite(actionLed, HIGH);  // 点亮指示灯
//         Serial.println("S");    
//         delay(100);                    // 防止重复触发
//         digitalWrite(actionLed, LOW);
//       }

//       samplesRead = numSamples;  // 重置采样计数器
//     }
//   }
//     int state = digitalRead(S_punch);

//   // 发送信号（按下时发送字符）
//   if (state == LOW) {
//     Serial.write('S');  // 发送轻拳信号
    
//     delay(50);          // 防抖延迟
//   }
// }// 定义按钮引脚n   
// 在全局变量区域添加以下声明（添加在文件顶部附近）

#include <ArduinoBLE.h>
#include <LSM6DS3.h>
#include <Wire.h>
#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "model.h"  // 确保这是你的模型头文件

// BLE配置
BLEService motionService("12345678-1234-5678-9ABC-DEF12345678A");
BLEStringCharacteristic motionChar("ABCD1234-1234-5678-9ABC-DEF12345678A", 
                                  BLERead | BLENotify, 20);

// 硬件配置
const int STATUS_LED = LED_BUILTIN;

// TensorFlow Lite配置
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr int kTensorArenaSize = 16 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// 传感器配置
LSM6DS3 imu(I2C_MODE, 0x6A);
const float ACCEL_THRESHOLD = 2.5;
const int NUM_SAMPLES = 119;

// 系统状态
enum SystemState { STATE_IDLE, STATE_COLLECTING, STATE_PROCESSING };
SystemState currentState = STATE_IDLE;
int samplesCollected = 0;

void setup() {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  Serial.begin(115200);
  while (!Serial);

  // 初始化BLE
  if (!BLE.begin()) {
    Serial.println("BLE初始化失败!");
    while (1) {
      digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
      delay(100);
    }
  }
  
  BLE.setLocalName("LeftHand");
  BLE.setAdvertisedService(motionService);
  motionService.addCharacteristic(motionChar);
  BLE.addService(motionService);
  BLE.advertise();

  // 初始化IMU
  if (imu.begin() != 0) {
    Serial.println("IMU初始化失败");
    while (1) {
      digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
      delay(500);
    }
  }

  // 初始化TensorFlow Lite
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("模型版本不匹配");
    while (1);
  }

  static tflite::MicroMutableOpResolver<5> resolver;
  resolver.AddFullyConnected();
  resolver.AddRelu();
  resolver.AddSoftmax();
  resolver.AddConv2D();
  resolver.AddAveragePool2D();

  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("分配张量失败");
    while (1);
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  digitalWrite(STATUS_LED, HIGH);
  Serial.println("系统就绪");
}

void loop() {
  BLE.poll();

  switch (currentState) {
    case STATE_IDLE:
      checkAcceleration();
      break;
    case STATE_COLLECTING:
      collectData();
      break;
    case STATE_PROCESSING:
      processModel();
      break;
  }

  // BLE连接状态指示
  static bool wasConnected = false;
  if (BLE.connected() != wasConnected) {
    wasConnected = !wasConnected;
    digitalWrite(STATUS_LED, wasConnected ? HIGH : LOW);
  }
}

void checkAcceleration() {
  static float avg[3] = {0};
  float aX = imu.readFloatAccelX();
  float aY = imu.readFloatAccelY();
  float aZ = imu.readFloatAccelZ();

  // 移动平均滤波
  avg[0] = avg[0] * 0.8 + aX * 0.2;
  avg[1] = avg[1] * 0.8 + aY * 0.2;
  avg[2] = avg[2] * 0.8 + aZ * 0.2;

  float sum = fabs(avg[0]) + fabs(avg[1]) + fabs(avg[2]);
  if (sum >= ACCEL_THRESHOLD) {
    currentState = STATE_COLLECTING;
    samplesCollected = 0;
  }
}

void collectData() {
  static unsigned long lastSample = 0;
  if (millis() - lastSample < 10) return;
  lastSample = millis();

  float* input_data = input->data.f;
  
  input_data[samplesCollected * 6 + 0] = imu.readFloatAccelX();
  input_data[samplesCollected * 6 + 1] = imu.readFloatAccelY();
  input_data[samplesCollected * 6 + 2] = imu.readFloatAccelZ();
  input_data[samplesCollected * 6 + 3] = imu.readFloatGyroX();
  input_data[samplesCollected * 6 + 4] = imu.readFloatGyroY();
  input_data[samplesCollected * 6 + 5] = imu.readFloatGyroZ();

  if (++samplesCollected >= NUM_SAMPLES) {
    currentState = STATE_PROCESSING;
  }
}

void processModel() {
  TfLiteStatus status = interpreter->Invoke();
  
  if (status != kTfLiteOk) {
    Serial.println("推理失败");
    currentState = STATE_IDLE;
    return;
  }

  float punchConf = output->data.f[0];
  float flexConf = output->data.f[1];

  if (punchConf > 0.4) {
    sendBLEValue("W");
  } else if (flexConf > 0.4) {
    sendBLEValue("S");
  }

  currentState = STATE_IDLE;
}

void sendBLEValue(const char* value) {
  if (BLE.connected()) {
    motionChar.writeValue(value);
    Serial.print("发送: ");
    Serial.println(value);
  }
}

