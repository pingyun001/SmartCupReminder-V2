#!/usr/bin/env python3
"""
STM32 HEX文件合并工具
用法: python merge_hex.py <文件1> <文件2> <输出文件> [--overwrite True/False]
"""

import argparse
import sys
import os
from intelhex import IntelHex

def merge_hex_files(file1, file2, output_file, overwrite=True):
    """
    合并两个HEX文件
    
    Args:
        file1: 第一个HEX文件路径
        file2: 第二个HEX文件路径  
        output_file: 输出文件路径
        overwrite: 如果地址冲突，是否用file2覆盖file1（默认覆盖）
    """
    try:
        # 检查输入文件是否存在
        if not os.path.exists(file1):
            raise FileNotFoundError(f"文件不存在: {file1}")
        if not os.path.exists(file2):
            raise FileNotFoundError(f"文件不存在: {file2}")
        
        print(f"正在合并HEX文件...")
        print(f"文件1: {file1}")
        print(f"文件2: {file2}")
        print(f"输出: {output_file}")
        print(f"覆盖策略: {'file2覆盖file1' if overwrite else '忽略冲突'}")
        print("-" * 50)
        
        # 加载HEX文件
        ih1 = IntelHex(file1)
        ih2 = IntelHex(file2)
        
        # 显示文件信息
        addresses1 = list(ih1.addresses())
        addresses2 = list(ih2.addresses())
        
        if addresses1:
            min_addr1 = min(addresses1)
            max_addr1 = max(addresses1)
            print(f"文件1地址范围: 0x{min_addr1:08X} - 0x{max_addr1:08X}")
        
        if addresses2:
            min_addr2 = min(addresses2)
            max_addr2 = max(addresses2)
            print(f"文件2地址范围: 0x{min_addr2:08X} - 0x{max_addr2:08X}")
        
        # 检查地址重叠
        overlap = set(addresses1) & set(addresses2)
        if overlap:
            print(f"警告: 发现 {len(overlap)} 个重叠地址")
            if overwrite:
                print("重叠区域将被文件2覆盖")
            else:
                print("重叠区域将保持文件1的内容")
        
        # 合并文件
        ih1.merge(ih2, overlap='replace' if overwrite else 'ignore')
        
        # 保存合并后的文件
        ih1.write_hex_file(output_file)
        
        # 显示合并结果
        merged_addresses = list(ih1.addresses())
        if merged_addresses:
            min_merged = min(merged_addresses)
            max_merged = max(merged_addresses)
            print(f"合并后地址范围: 0x{min_merged:08X} - 0x{max_merged:08X}")
        
        print(f"合并成功！输出文件: {output_file}")
        print(f"文件大小: {os.path.getsize(output_file)} 字节")
        
    except Exception as e:
        print(f"合并失败: {e}")
        sys.exit(1)

def main():
    """主函数：解析命令行参数并执行合并"""
    parser = argparse.ArgumentParser(
        description="合并两个STM32 HEX固件文件",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python merge_hex.py bootloader.hex application.hex firmware.hex
  python merge_hex.py f1.hex f2.hex output.hex --overwrite False
  python merge_hex.py /path/to/file1.hex /path/to/file2.hex /path/to/merged.hex
        """
    )
    
    parser.add_argument('file1', help='第一个HEX文件路径')
    parser.add_argument('file2', help='第二个HEX文件路径')
    parser.add_argument('output', help='合并后的输出文件路径')
    parser.add_argument('--overwrite', type=bool, default=True, 
                       help='地址冲突时是否覆盖(True/False，默认为True)')
    
    # 解析参数
    args = parser.parse_args()
    
    # 执行合并
    merge_hex_files(args.file1, args.file2, args.output, args.overwrite)

if __name__ == "__main__":
    main()