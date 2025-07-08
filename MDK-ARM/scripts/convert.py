import struct

# 函数：浮点数转4字节数组（小端序）
def float_to_bytes(value):
    # 使用格式符 '<f'：小端序（<），32位浮点数（f）
    return struct.pack('<f', value)

# 示例用法：将线速度转换为字节数组
linear_velocity = 0.8
bytes_data = float_to_bytes(linear_velocity)  # 输出为b'\x00\x00\x9a@'（示例值）
print(bytes_data)