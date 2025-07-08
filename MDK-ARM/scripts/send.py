import struct
import serial

# 修改端口号为'COM22'，波特率保持与树莓派一致（115200）
ser = serial.Serial('COM22', 921600, timeout=0.1)  

def send_velocity(v_linear, v_angular):
    # 构建帧格式与之前一致：帧头(0xaa) + 线速度(float) + 角速度(float) + 帧尾(0xbb)
    # 使用小端字节序（<），对应格式字符：B=无符号字节，f=32位浮点数
    frame = struct.pack('<BffB', 0xaa, v_linear, v_angular, 0xbb)
    ser.write(frame)  # 发送数据帧
    ser.flush()       # 确保数据立即发送