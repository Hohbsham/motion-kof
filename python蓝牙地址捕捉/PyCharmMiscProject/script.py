import serial
import pyautogui
import time

# # 配置串口参数（根据实际端口修改）
# ser = serial.Serial('COM9', 9600)
# pyautogui.PAUSE = 0.01            # 降低按键延迟
#
# # WinKawaks 按键映射（需与模拟器设置一致）
# KEY_MAP = {
#     'S': 'j',  # 轻拳（假设模拟器中 j=轻拳）
#     'B': 'k'   # 重脚（假设模拟器中 k=重脚）
# }
#
# # 修改后的 Python 处理函数
# def process_signal(byte):
#     try:
#         # 将整数转换为对应的 ASCII 字符
#         char = chr(byte)  # 将整数转换为字符
#         if char.strip() == 'S':
#             # 执行必杀技操作（例如↓→ + 拳）
#             pyautogui.keyDown('j')
#             time.sleep(0.1)
#             pyautogui.keyUp('j')
#         if char.strip() == 'B':
#
#             pyautogui.keyDown('K')
#             time.sleep(0.1)
#             pyautogui.keyUp('K')
#     except Exception as e:
#         print(f"处理信号时出错：{e}")
#
# try:
#     print("等待 Arduino 信号...")
#     while True:
#         if ser.in_waiting > 0:
#             data = ser.read(ser.in_waiting)  # 读取所有可用数据
#             for byte in data:  # 遍历每个字节（整数）
#                 process_signal(byte)  # 处理单个字节
# except KeyboardInterrupt:
#     ser.close()
#
#     print("程序已退出")
import asyncio
from bleak import BleakScanner

async def scan_ble_devices():
    print("正在扫描附近的蓝牙设备，请稍等...")
    devices = await BleakScanner.discover(timeout=10)  # timeout: 扫描时间，单位秒

    if not devices:
        print("没有发现任何蓝牙设备")
        return

    print(f"\n✅ 共发现 {len(devices)} 个蓝牙设备：\n")
    for d in devices:
        name = d.name or "Unknown Device"
        address = d.address
        rssi = d.rssi
        print(f"设备名: {name:<20} MAC地址: {address:<20} RSSI: {rssi} dBm")

if __name__ == "__main__":
    asyncio.run(scan_ble_devices())