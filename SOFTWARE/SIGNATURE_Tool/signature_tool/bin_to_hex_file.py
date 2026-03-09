#!/usr/bin/env python3
"""
BIN 转 HEX 转换器
使用方法: python bin2hex.py 输入文件.bin [起始地址]
起始地址可以是十六进制(0x开头)或十进制
示例: python bin2hex.py firmware.bin 0x08010000
"""

import sys
import os
from intelhex import IntelHex

def parse_address(addr_str):
    """解析地址字符串，支持十六进制和十进制"""
    addr_str = str(addr_str).strip()
    
    if addr_str.startswith('0x') or addr_str.startswith('0X'):
        # 十六进制
        try:
            return int(addr_str, 16)
        except ValueError:
            raise ValueError(f"无效的十六进制地址: {addr_str}")
    else:
        # 十进制
        try:
            return int(addr_str)
        except ValueError:
            raise ValueError(f"无效的十进制地址: {addr_str}")

def bin_to_hex(bin_file, start_address=0x08000000):
    """将BIN文件转换为HEX文件"""
    
    if not os.path.exists(bin_file):
        print(f"错误: 文件不存在 - {bin_file}")
        return False
    
    # 分离文件名和扩展名
    file_dir, file_fullname = os.path.split(bin_file)
    file_name, file_ext = os.path.splitext(file_fullname)
    
    # 构建输出文件路径
    if file_dir:
        hex_file = os.path.join(file_dir, f"{file_name}.hex")
    else:
        hex_file = f"{file_name}.hex"
    
    print(f"输入文件: {bin_file}")
    print(f"输出文件: {hex_file}")
    print(f"起始地址: 0x{start_address:08X} ({start_address})")
    
    try:
        # 创建IntelHex对象
        ih = IntelHex()
        
        # 加载BIN文件到指定地址
        ih.loadbin(bin_file, offset=start_address)
        
        # 保存为HEX文件
        ih.tofile(hex_file, format='hex')
        
        # 获取文件信息
        bin_size = os.path.getsize(bin_file)
        min_addr = ih.minaddr()
        max_addr = ih.maxaddr()
        
        print("转换完成!")
        print(f"BIN文件大小: {bin_size} 字节 (0x{bin_size:X})")
        print(f"HEX起始地址: 0x{min_addr:08X}")
        print(f"HEX结束地址: 0x{max_addr:08X}")
        print(f"地址范围大小: {max_addr - min_addr + 1} 字节")
        
        return True
        
    except Exception as e:
        print(f"转换失败: {e}")
        return False

def main():
    """主函数"""
    # 默认STM32 Flash起始地址
    DEFAULT_START_ADDRESS = 0x08000000
    
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("BIN 转 HEX 转换器")
        print(f"默认起始地址: 0x{DEFAULT_START_ADDRESS:08X} (STM32 Flash起始地址)")
        print("使用方法:")
        print("  python bin2hex.py 输入文件.bin [起始地址]")
        print("  起始地址可以是十六进制(0x开头)或十进制")
        print("示例:")
        print(f"  python bin2hex.py firmware.bin                (使用默认地址 0x{DEFAULT_START_ADDRESS:08X})")
        print("  python bin2hex.py firmware.bin 0x08010000")
        print("  python bin2hex.py firmware.bin 134217728       (0x08010000的十进制)")
        return
    
    bin_file = sys.argv[1]
    
    # 处理起始地址参数
    if len(sys.argv) == 3:
        try:
            start_address = parse_address(sys.argv[2])
        except ValueError as e:
            print(f"错误: {e}")
            return
    else:
        start_address = DEFAULT_START_ADDRESS
    
    # 验证起始地址范围
    if start_address < 0:
        print(f"错误: 起始地址不能为负数: 0x{start_address:08X}")
        return
    
    success = bin_to_hex(bin_file, start_address)
    
    if not success:
        sys.exit(1)

if __name__ == "__main__":
    main()