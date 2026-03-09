@echo off
setlocal enabledelayedexpansion

set "INPUT_DIR=input"
set "TARGET_FILE=bootloader.hex"

:: 检查input文件夹和目标文件是否存在
if not exist "%INPUT_DIR%\%TARGET_FILE%" (
    echo Error: %INPUT_DIR%\%TARGET_FILE% not found
    pause
    exit /b 1
)

:: 获取当前input文件夹中bootloader.hex的修改时间
for %%I in ("%INPUT_DIR%\%TARGET_FILE%") do set "INPUT_TIME=%%~tI"

echo Current %TARGET_FILE% in %INPUT_DIR%: %INPUT_TIME%

:: 在上级目录中搜索bootloader.hex文件
set "NEWEST_FILE="
set "NEWEST_TIME="

for /r ".." %%F in (%TARGET_FILE%) do (
    if exist "%%F" (
        for %%I in ("%%F") do (
            set "FILE_TIME=%%~tI"
            if "!NEWEST_TIME!" lss "!FILE_TIME!" (
                set "NEWEST_TIME=!FILE_TIME!"
                set "NEWEST_FILE=%%F"
            )
        )
    )
)

:: 检查是否找到了更新的文件
if defined NEWEST_FILE (
    echo Found newer %TARGET_FILE%: !NEWEST_TIME!
    echo Location: !NEWEST_FILE!
    
    :: 比较时间戳
    if "!INPUT_TIME!" lss "!NEWEST_TIME!" (
        echo Updating %TARGET_FILE%...
        copy /Y "!NEWEST_FILE!" "%INPUT_DIR%\%TARGET_FILE%"
        echo Update completed
    ) else (
        echo No newer version found
    )
) else (
    echo No %TARGET_FILE% found in parent directories
)

pause