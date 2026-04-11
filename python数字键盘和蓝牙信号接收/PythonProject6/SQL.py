import asyncio
import time
from bleak import BleakClient, BleakError
import pyautogui

# BLE配置参数
DEVICES = [
    {"address": "01:C0:A0:45:FB:4C", "service_uuid": "12345678-1234-5678-9ABC-DEF123456789",
     "char_uuid": "ABCD1234-1234-5678-9ABC-DEF123456789"},
    {"address": "24:68:87:AC:B7:AC", "service_uuid": "12345678-1234-5678-9ABC-DEF12345678A",
     "char_uuid": "ABCD1234-1234-5678-9ABC-DEF12345678A"},
    {"address": "D6:8F:87:53:0A:3A", "service_uuid": "12345678-1234-5678-9ABC-DEF12345678B",
     "char_uuid": "ABCD1234-1234-5678-9ABC-DEF12345678B"},
    {"address": "4F:DB:92:81:6B:B6", "service_uuid": "12345678-1234-5678-9ABC-DEF12345678C",
     "char_uuid": "ABCD1234-1234-5678-9ABC-DEF12345678C"}
]



# 动作映射配置（保留你的原始映射）
ACTION_MAPPING = {
    # 方向控制（短按）
    'W': {'type': 'direction', 'key': 'w', 'duration': 0.5},  # 前
    'S': {'type': 'direction', 'key': 's', 'duration': 0.2},  # 后
    'A': {'type': 'direction', 'key': 'a', 'duration': 0.2},  # 跳
    'D': {'type': 'direction', 'key': 'd', 'duration': 0.2},  # 蹲

    # 攻击按钮
    'J': {'type': 'attack', 'key': 'j', 'duration': 0.2},  # 轻拳
    'K': {'type': 'attack', 'key': 'k', 'duration': 0.2},  # 轻腿
    'L': {'type': 'attack', 'key': 'l', 'duration': 0.2},  # 重拳
    'U': {'type': 'attack', 'key': 'u', 'duration': 0.2},  # 重腿
}

class BLEClient:
    def __init__(self, device_info):
        self.device_info = device_info
        self.client = None
        self.connected = False
        self.reconnect_attempts = 0
        self.max_reconnect_attempts = 5
        self.shutdown = False

    def notification_callback(self, sender, data):
        """处理蓝牙通知数据"""
        try:
            decoded = data.decode("utf-8").strip().upper()
            print(f"[{time.strftime('%H:%M:%S')}] 设备 {self.device_info['address']} 收到指令: {decoded}")
            self.handle_keyboard_input(decoded)

        except UnicodeDecodeError:
            print(f"[{time.strftime('%H:%M:%S')}] 设备 {self.device_info['address']} 收到非文本数据: {bytes(data)}")
        except Exception as e:
            print(f"设备 {self.device_info['address']} 处理数据时发生错误: {str(e)}")

    def handle_keyboard_input(self, command):
        """执行键盘操作"""
        key_config = ACTION_MAPPING.get(command)
        if not key_config:
            print(f"设备 {self.device_info['address']} 忽略未定义指令: {command}")
            return

        key = key_config['key']
        duration = key_config['duration']

        try:
            # 模拟按键按下 + 等待释放
            pyautogui.keyDown(key)
            time.sleep(duration)
            pyautogui.keyUp(key)
            print(f"设备 {self.device_info['address']} 已触发按键: {key.upper()}")
        except pyautogui.FailSafeException:
            print("安全限制：请确保鼠标在屏幕安全区域")
        except Exception as e:
            print(f"设备 {self.device_info['address']} 键盘操作失败: {str(e)}")

    async def connect_with_retry(self):
        """带自动重连的连接方法"""
        while not self.shutdown and self.reconnect_attempts < self.max_reconnect_attempts:
            try:
                print(
                    f"设备 {self.device_info['address']} 连接尝试 ({self.reconnect_attempts + 1}/{self.max_reconnect_attempts})...")
                self.client = BleakClient(self.device_info["address"])
                await self.client.connect()
                self.connected = True
                self.reconnect_attempts = 0
                print(f"设备 {self.device_info['address']} 蓝牙连接成功！")

                await self.client.start_notify(self.device_info["char_uuid"], self.notification_callback)
                print(f"设备 {self.device_info['address']} 通知监听已启用")

                while self.connected and not self.shutdown:
                    await asyncio.sleep(1)

            except BleakError as e:
                print(f"设备 {self.device_info['address']} 连接失败: {str(e)}")
                self.reconnect_attempts += 1
                await asyncio.sleep(2 ** self.reconnect_attempts)  # 指数退避
            finally:
                self.connected = False
                if self.client and self.client.is_connected:
                    await self.client.stop_notify(self.device_info["char_uuid"])
                    await self.client.disconnect()
                print(f"设备 {self.device_info['address']} 连接已断开")

    async def user_interface(self):
        """用户控制界面"""
        loop = asyncio.get_event_loop()
        while not self.shutdown:
            try:
                user_input = await loop.run_in_executor(None, input, "输入Q退出程序: ")
                if user_input.strip().upper() == 'Q':
                    self.shutdown = True
                    print("正在关闭...")
                    break
            except Exception as e:
                print(f"输入错误: {str(e)}")


async def main():
    clients = [BLEClient(device) for device in DEVICES]

    # 运行并行任务
    try:
        await asyncio.gather(
            *(client.connect_with_retry() for client in clients),
            clients[0].user_interface()  # 使用第一个客户端的用户界面
        )
    except Exception as e:
        print(f"程序异常: {str(e)}")
    finally:
        for client in clients:
            client.shutdown = True
            if client.client and client.client.is_connected:
                await client.client.disconnect()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n程序已终止")