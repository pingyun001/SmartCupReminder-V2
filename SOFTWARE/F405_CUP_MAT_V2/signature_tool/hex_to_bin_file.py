#!/usr/bin/env python3
"""
HEX 转 BIN 转换器
使用方法: python hex2bin.py 输入文件.hex
会在相同目录下生成 输入文件.bin
"""

import sys
import os
from intelhex import IntelHex

def hex_to_bin(hex_file):
    """将HEX文件转换为BIN文件"""
    
    if not os.path.exists(hex_file):
        print(f"错误: 文件不存在 - {hex_file}")
        return False
    
    # 分离文件名和扩展名
    file_dir, file_fullname = os.path.split(hex_file)
    file_name, file_ext = os.path.splitext(file_fullname)
    
    # 构建输出文件路径
    if file_dir:
        bin_file = os.path.join(file_dir, f"{file_name}.bin")
    else:
        bin_file = f"{file_name}.bin"
    
    print(f"输入文件: {hex_file}")
    print(f"输出文件: {bin_file}")
    
    try:
        # 加载HEX文件
        ih = IntelHex(hex_file)
        
        # 转换为BIN文件
        ih.tobinfile(bin_file)
        
        # 显示一些信息
        min_addr = ih.minaddr()
        max_addr = ih.maxaddr()
        file_size = max_addr - min_addr + 1
        
        print("转换完成!")
        print(f"起始地址: 0x{min_addr:08X}")
        print(f"结束地址: 0x{max_addr:08X}")
        print(f"文件大小: {file_size} 字节 (0x{file_size:X})")
        
        return True
        
    except Exception as e:
        print(f"转换失败: {e}")
        return False

def main():
    """主函数"""
    if len(sys.argv) != 2:
        print("HEX 转 BIN 转换器")
        print("使用方法: python hex2bin.py 输入文件.hex")
        print("示例: python hex2bin.py firmware.hex")
        return
    
    hex_file = sys.argv[1]
    success = hex_to_bin(hex_file)
    
    if not success:
        sys.exit(1)

if __name__ == "__main__":
    main()