# V2.1 固件说明
新增bootloader + app功能，若非特殊说明，该bootloader适用未来所有版本固件
## 概述
芯片型号：STM32F405RG
- 固件起始地址：0x08010000
  

# 下载方式
推荐使用STM32CubeMX工具下载，可通过ST-Link或USB DFU下载。

## 方案1：U盘自动更新
> 该方案适用于所有运行在V2.1及以上版本固件的杯垫，旧版V2.0及以下版本固件不适用。

1. 寻找心仪的固件（固件列表里后缀为.bin的文件），将其改名为update.bin，并记住位置
2. 进入U盘模式，将需要更新的固件文件（update.bin）放入U盘根目录，待传输完成后，弹出U盘
3. 为系统重新上电，等待左上角LED灯点亮
4. 若LED为黄色，则成功识别到待更新固件，并开始尝试更新
5. 若更新成功，则LED灯变绿色，之后系统会自动运行新固件


## 方案2：ST-Link
> 该方案最为稳定，也是最推荐的方案，但是需要额外准备一个ST-Link调试器。

连接ST-Link SWD接口到PCB板，推荐按如下方式接线：

左侧为ST-Link接口，右侧为PCB接口：
- 3V3   <---> 3V3
- SWCLK <---> SCK
- GND   <---> GND
- SWDIO <---> SDA
- RESET <---> RST

同时，建议断开USB供电，全板仅通过ST-Link提供STM32F4所需的3.3V电源

完成连接后，将ST-Link插入电脑，打开STM32CubeProgrammer软件，选择ST-Link模式，点击Connect按钮，选择下载的固件文件（.hex后缀），点击Program按钮，等待下载完成。

## 方案3：USB DFU
> 该方案有概率失败（受限于USB线材质量，焊接质量，以及STM32F4系列众所周知的USB bug，有概率无法被电脑识别）

按住PCB上的ST-BOOT按键 不松开，通过USB-TypeC线连接PCB板和电脑，打开STM32CubeProgrammer软件，选择USB DFU模式，点击Connect按钮，选择下载的固件文件，点击Program按钮，等待下载完成。

# 版权
bilibili 平韵の小窝 原创作品

仅限个人学习使用，严禁无授权的商业用途！
