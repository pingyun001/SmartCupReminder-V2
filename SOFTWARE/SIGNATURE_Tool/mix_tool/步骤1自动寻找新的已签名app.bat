@echo off
setlocal enabledelayedexpansion

set "INPUT_DIR=input"
set "SIGNED_FIRMWARE_DIR=..\signature_tool\firmware"
set "BOOTLOADER_NAME=bootloader.hex"

:: 检查input文件夹是否存在
if not exist "%INPUT_DIR%" (
    echo Error: %INPUT_DIR% folder not found
    pause
    exit /b 1
)

:: 检查签名固件目录是否存在
if not exist "%SIGNED_FIRMWARE_DIR%" (
    echo Error: %SIGNED_FIRMWARE_DIR% folder not found
    pause
    exit /b 1
)

:: 查找签名固件目录中的已签名hex文件
set "SIGNED_FILE="
for /r "%SIGNED_FIRMWARE_DIR%" %%F in (*_signed.hex) do (
    set "SIGNED_FILE=%%F"
    goto :found_signed
)

echo No *_signed.hex file found in %SIGNED_FIRMWARE_DIR%
pause
exit /b 1

:found_signed
echo Found signed firmware: !SIGNED_FILE!

:: 查找input文件夹中非bootloader.hex的hex文件
set "TARGET_FILE="
for %%F in ("%INPUT_DIR%\*.hex") do (
    if /i not "%%~nxF"=="%BOOTLOADER_NAME%" (
        set "TARGET_FILE=%%F"
        goto :replace_file
    )
)

:: 如果没有找到目标文件，直接将签名固件复制到input文件夹
echo No non-bootloader hex file found, copying signed firmware to %INPUT_DIR%
copy "!SIGNED_FILE!" "%INPUT_DIR%\"
echo Copy completed
pause
exit /b 0

:replace_file
echo Replacing !TARGET_FILE! with !SIGNED_FILE!
copy /Y "!SIGNED_FILE!" "!TARGET_FILE!"
echo Replacement completed

pause