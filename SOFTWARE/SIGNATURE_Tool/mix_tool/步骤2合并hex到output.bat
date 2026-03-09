@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: 设置路径
set "INPUT_DIR=input"
set "OUTPUT_DIR=output"
set "PYTHON_SCRIPT=merge_hex.py"

:: 检查Python是否安装
python --version >nul 2>&1
if errorlevel 1 (
    echo Error: Python not found
    pause
    exit /b 1
)

:: 检查input文件夹是否存在
if not exist "%INPUT_DIR%" (
    echo Error: Input folder not exist
    pause
    exit /b 1
)

:: 检查output文件夹是否存在，不存在则创建
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

:: 获取input文件夹中的hex文件
set "FILE_COUNT=0"
set "FILE1="
set "FILE2="

for %%f in ("%INPUT_DIR%\*.hex") do (
    set /a FILE_COUNT+=1
    if !FILE_COUNT! equ 1 set "FILE1=%%f"
    if !FILE_COUNT! equ 2 set "FILE2=%%f"
)

:: 检查文件数量
if %FILE_COUNT% equ 0 (
    echo Error: No hex files found
    pause
    exit /b 1
)

if %FILE_COUNT% equ 1 (
    echo Error: Only 1 hex file found, need 2 files
    pause
    exit /b 1
)

if %FILE_COUNT% gtr 2 (
    echo Warning: More than 2 hex files found, using first two
)

:: 生成简洁的输出文件名
for %%i in ("%FILE1%") do set "BASE_NAME1=%%~ni"
for %%i in ("%FILE2%") do set "BASE_NAME2=%%~ni"
set "OUTPUT_FILE=%OUTPUT_DIR%\merged.hex"

echo Files: %FILE1% + %FILE2%
echo Output: %OUTPUT_FILE%

:: 执行Python合并脚本
python "%PYTHON_SCRIPT%" "%FILE1%" "%FILE2%" "%OUTPUT_FILE%"

if errorlevel 1 (
    echo Merge failed
    pause
    exit /b 1
)

echo Merge successful: %OUTPUT_FILE%
pause