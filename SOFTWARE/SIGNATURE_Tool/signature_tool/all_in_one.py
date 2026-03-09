#!/usr/bin/env python3
"""
固件签名自动化脚本（集成版）
使用方法: python firmware_signer_integrated.py <hex文件> <版本号>
"""

import sys
import os
import subprocess
import struct
from pathlib import Path
from intelhex import IntelHex

def main():
    if len(sys.argv) != 3:
        print("使用方法: python firmware_signer_integrated.py <hex文件> <版本号>")
        print("示例: python firmware_signer_integrated.py demo.hex 0x12345678")
        return 1
    
    hex_file = sys.argv[1]
    version = sys.argv[2]
    
    if not os.path.exists(hex_file):
        print(f"错误: 文件不存在 - {hex_file}")
        return 1
    
    print("=" * 50)
    print("固件签名自动化流程（集成版）")
    print("=" * 50)
    
    # 步骤1: HEX转BIN
    print("[1/3] HEX -> BIN...")
    hex_path = Path(hex_file)
    bin_file = hex_path.with_suffix(".bin")
    
    try:
        ih = IntelHex(hex_file)
        ih.tobinfile(bin_file)
        print(f"  ✓ 生成: {bin_file.name} ({bin_file.stat().st_size} 字节)")
    except Exception as e:
        print(f"  ✗ 失败: {e}")
        return 1
    
    # 步骤2: 固件签名
    print("[2/3] 固件签名...")
    signed_bin = bin_file.parent / f"{bin_file.stem}_signed{bin_file.suffix}"
    
    cmd = ["signature.exe", str(bin_file), version]
    print(f"  执行: {' '.join(cmd)}")
    
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        if result.stdout:
            print(f"  输出: {result.stdout.strip()}")
    except subprocess.CalledProcessError as e:
        print(f"  ✗ 失败: {e.stderr}")
        return 1
    except FileNotFoundError:
        print(f"  ✗ 失败: 未找到signature.exe")
        return 1
    
    if not signed_bin.exists():
        print(f"  ✗ 失败: 未生成签名文件")
        return 1
    
    print(f"  ✓ 生成: {signed_bin.name} ({signed_bin.stat().st_size} 字节)")
    
    # 步骤3: BIN转HEX
    print("[3/3] BIN -> HEX...")
    signed_hex = signed_bin.with_suffix(".hex")
    
    try:
        ih = IntelHex()
        ih.loadbin(signed_bin, offset=0x08010000)
        ih.tofile(signed_hex, format='hex')
        print(f"  ✓ 生成: {signed_hex.name} ({signed_hex.stat().st_size} 字节)")
    except Exception as e:
        print(f"  ✗ 失败: {e}")
        return 1
    
    print("=" * 50)
    print("✓ 签名流程完成!")
    print(f"最终文件: {signed_hex}")
    print("=" * 50)
    
    return 0

if __name__ == "__main__":
    sys.exit(main())