# 功能介绍
一个简单的签名工具，用于为提醒喝水杯垫的APP固件进行签名，以便被bootloader自动识别并校验更新固件。
# 如何使用

## 环境搭建
本项目需使用cmake构建，请确保系统中已安装cmake

测试方法

```
cmake --version
```
如果输出信息如下：
```
cmake version 3.31.0-rc3

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```
表示CMake环境正常

测试make环境
```
make --version
```
如果输出信息如下：
```
GNU Make 4.4.1
Built for x86_64-w64-mingw32
Copyright (C) 1988-2023 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
```
表示make环境正常
## 编译工程
使用VSCode打开工程
安装C/C++扩展，并配置好编译环境

右键CMakeLists.txt，选择清理所有项目并重新配置

点击VSCode左下角的三角按钮，开始编译并运行

## 效果
编译成功后，在build目录下生成签名工具exe可执行文件，可通过命令行测试该工具

# 版权
bilibili 平韵の小窝 原创作品

仅限个人学习使用，严禁无授权的商业用途！

