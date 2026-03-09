# 功能介绍
将多个文件打包到FatFs bin，将该bin下载到Flash，以便被嵌入式系统挂载并使用
# 如何使用

## 获取源码
```
git clone https://github.com/pingyun001/FatFsBin.git
```

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
## 编译FatFsBin
使用VSCode打开工程
安装C/C++扩展，并配置好编译环境

右键CMakeLists.txt，选择清理所有项目并重新配置

点击VSCode左下角的三角按钮，开始编译并运行

## 效果
编译成功后，在build目录下生成SimFatFs.exe可执行文件，该文件即为FatFs的模拟器，可以用来测试FatFs是否正确运行

在.\workingSpace目录下生成fsft.bin，该文件即为FatFs bin文件，可以将其下载到Flash中，并在嵌入式系统中挂载使用

该bin中会将.\workingSpace\filesToBePackd中内容打包（暂不支持嵌套文件夹）

# 测试
将bin文件下载到Flash中，并将其挂载到系统中，使用FatFs的相关接口，读取文件内容，验证是否正确

# 注意事项
1. 该项目仅支持Windows系统，Linux系统请自行修改相关配置文件
2. 生成的bin文件大小在main.h中SIM_BUFFER_SIZE修改，默认4MB，请确保足够存放待打包文件
3. 该项目仅支持单个文件打包，不支持文件夹打包