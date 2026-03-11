#!/usr/bin/env python3
"""
Firmware Signature Automation Script
Usage: python firmware_signer.py <hex_file> <version>
Example: python firmware_signer.py demo.hex 0x12345678
"""

import sys
import os
import subprocess
import argparse
import shutil
from pathlib import Path

def get_script_dir():
    """Get the absolute path of the script directory"""
    return Path(__file__).parent.absolute()

def parse_hex_start_address(hex_file):
    """Parse the start address directly from HEX file without IntelHex library"""
    try:
        hex_path = str(hex_file)
        print(f"  Parsing HEX file: {hex_path}")
        print(f"  File exists: {os.path.exists(hex_path)}")
        print(f"  File size: {os.path.getsize(hex_path)} bytes")
        
        with open(hex_path, 'r') as f:
            first_line = f.readline().strip()
            
        print(f"  First line: {first_line}")
        
        # HEX file format: :LLAAAATTDD...CC
        # Where LL = length, AAAA = address, TT = record type
        
        if not first_line.startswith(':'):
            print("  Error: Not a valid HEX file (doesn't start with ':')")
            return None
        
        # Extract address from the first line
        # Skip the leading ':' and get the address field (bytes 1-4, hex)
        addr_str = first_line[3:7]  # Positions 3-6 (0-based, after the ':')
        
        try:
            # Convert hex string to integer
            start_addr = int(addr_str, 16)
            print(f"  Parsed start address: 0x{start_addr:04X}")
            
            # Check if this is an extended address record
            record_type = first_line[7:9]
            if record_type == '04':
                # Extended linear address record
                high_addr_str = first_line[9:13]
                high_addr = int(high_addr_str, 16)
                start_addr = (high_addr << 16) | start_addr
                print(f"  Extended address record, high part: 0x{high_addr:04X}")
                print(f"  Full start address: 0x{start_addr:08X}")
            elif record_type == '02':
                # Extended segment address record
                high_addr_str = first_line[9:13]
                high_addr = int(high_addr_str, 16)
                start_addr = (high_addr << 4) | start_addr
                print(f"  Segment address record, high part: 0x{high_addr:04X}")
                print(f"  Full start address: 0x{start_addr:08X}")
            
            return start_addr
            
        except ValueError as e:
            print(f"  Error parsing address: {e}")
            return None
            
    except Exception as e:
        print(f"  Error reading HEX file: {type(e).__name__}: {e}")
        return None

def get_hex_start_address(hex_file):
    """Try multiple methods to get the start address from a HEX file"""
    # First try with IntelHex library
    try:
        from intelhex import IntelHex
        print(f"  Trying IntelHex library...")
        ih = IntelHex(str(hex_file))
        min_addr = ih.minaddr()
        print(f"  IntelHex found start address: 0x{min_addr:08X}")
        return min_addr
    except Exception as e:
        print(f"  IntelHex failed: {type(e).__name__}: {e}")
    
    # Fall back to manual parsing
    print(f"  Falling back to manual HEX parsing...")
    return parse_hex_start_address(hex_file)

def hex_to_bin(hex_file, script_dir):
    """Execute HEX to BIN conversion"""
    print(f"[1/4] HEX -> BIN: {hex_file.name}")
    
    # First get the start address from the HEX file
    hex_start_addr = get_hex_start_address(hex_file)
    if hex_start_addr is None:
        hex_start_addr = 0x08010000
        print(f"  Using default start address: 0x{hex_start_addr:08X}")
    else:
        print(f"  Found HEX start address: 0x{hex_start_addr:08X}")
    
    hex2bin_script = script_dir / "hex_to_bin_file.py"
    if not hex2bin_script.exists():
        print(f"Error: Script not found {hex2bin_script}")
        return None, None
    
    bin_file = hex_file.with_suffix(".bin")
    
    cmd = ["python", str(hex2bin_script), str(hex_file)]
    
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: HEX to BIN conversion failed")
        return None, None
    
    if not bin_file.exists():
        print(f"Error: BIN file not generated")
        return None, None
    
    file_size = bin_file.stat().st_size
    print(f"  OK Generated: {bin_file.name} ({file_size} bytes)")
    
    return bin_file, hex_start_addr

def sign_bin(bin_file, version, script_dir):
    """Execute firmware signing"""
    print(f"[2/4] Firmware Signing: {bin_file.name}")
    
    signature_tool = script_dir / "signature.exe"
    if not signature_tool.exists():
        print(f"Error: Signature tool not found")
        return None
    
    signed_bin = bin_file.parent / f"{bin_file.stem}_signed{bin_file.suffix}"
    
    cmd = [str(signature_tool), str(bin_file), str(version)]
    
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: Firmware signing failed")
        return None
    
    if not signed_bin.exists():
        print(f"Error: Signed BIN file not generated")
        return None
    
    file_size = signed_bin.stat().st_size
    print(f"  OK Generated: {signed_bin.name} ({file_size} bytes)")
    
    return signed_bin

def bin_to_hex(bin_file, start_addr, script_dir):
    """Execute BIN to HEX conversion"""
    print(f"[3/4] BIN -> HEX: {bin_file.name}")
    
    bin2hex_script = script_dir / "bin_to_hex_file.py"
    if not bin2hex_script.exists():
        print(f"Error: Script not found")
        return None
    
    hex_file = bin_file.with_suffix(".hex")
    
    cmd = ["python", str(bin2hex_script), str(bin_file), f"0x{start_addr:08X}"]
    print(f"  Using start address: 0x{start_addr:08X}")
    
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: BIN to HEX conversion failed")
        return None
    
    if not hex_file.exists():
        print(f"Error: HEX file not generated")
        return None
    
    file_size = hex_file.stat().st_size
    print(f"  OK Generated: {hex_file.name} ({file_size} bytes)")
    
    return hex_file

def replace_original_hex(signed_hex, original_hex_path):
    """Replace original HEX file with signed HEX file"""
    print(f"[4/4] Replace original HEX file: {original_hex_path.name}")
    
    try:
        # 备份原始文件
        backup_path = original_hex_path.with_suffix(".hex.backup")
        if original_hex_path.exists():
            shutil.copy2(original_hex_path, backup_path)
            print(f"  Backup created: {backup_path.name}")
        
        # 用签名后的HEX文件替换原始HEX文件
        shutil.copy2(signed_hex, original_hex_path)
        print(f"  OK Replaced: {original_hex_path.name} with signed version")
        
        # 显示文件大小
        new_size = original_hex_path.stat().st_size
        print(f"  New file size: {new_size} bytes")
        
        return original_hex_path
    except Exception as e:
        print(f"Error: Failed to replace file - {e}")
        return None

def main():
    """Main function"""
    script_dir = get_script_dir()
    
    parser = argparse.ArgumentParser(description="Firmware Signature Automation Script")
    parser.add_argument("hex_file", help="Input HEX file path")
    parser.add_argument("version", help="Firmware version")
    
    args = parser.parse_args()
    
    if not os.path.exists(args.hex_file):
        print(f"Error: File not found - {args.hex_file}")
        return 1
    
    print("Firmware Signature Process")
    print("=" * 50)
    
    # 保存原始文件路径
    original_hex_path = Path(args.hex_file)
    
    # Create firmware folder
    firmware_dir = script_dir / "firmware"
    firmware_dir.mkdir(exist_ok=True)
    
    # Copy HEX file to firmware folder
    dest_hex_path = firmware_dir / original_hex_path.name
    
    shutil.copy2(original_hex_path, dest_hex_path)
    print(f"Copied: {original_hex_path.name} -> firmware/")
    print()
    
    # Step 1: HEX to BIN, and get start address from HEX
    bin_file, hex_start_addr = hex_to_bin(dest_hex_path, script_dir)
    if not bin_file:
        return 1
    
    # Step 2: Firmware Signing
    signed_bin = sign_bin(bin_file, args.version, script_dir)
    if not signed_bin:
        return 1
    
    # Step 3: BIN to HEX, using the start address from original HEX
    signed_hex = bin_to_hex(signed_bin, hex_start_addr, script_dir)
    if not signed_hex:
        return 1
    
    # Step 4: Replace original HEX file with signed HEX
    result = replace_original_hex(signed_hex, original_hex_path)
    if not result:
        return 1
    
    print("=" * 50)
    print(f"Completed!")
    print(f"Original file replaced: {original_hex_path}")
    print(f"Signed file in workspace: firmware/{signed_hex.name}")
    print(f"Used start address: 0x{hex_start_addr:08X}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())