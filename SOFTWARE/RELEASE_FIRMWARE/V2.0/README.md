# V2.0 固件说明
该版本已弃用，仅作备份
## 概述
首次发布版本, 芯片型号：STM32F405RG
- F405_CUP_MAT_V2.0.hex: 
  - 固件起始地址：0x08000000
  - 固件总大小：839.33KB (81.9%)
  - SRAM占用：189.04KB (98.4%)
- F405_CUP_MAT_V2.0_DEBUG.hex:
  - 固件起始地址：0x08000000
  - 固件总大小：886.70KB (86.6%)
  - SRAM占用：189.11KB (98.5%)

# 下载方式
推荐使用STM32CubeMX工具下载，可通过ST-Link或USB DFU下载。
## 方案1：ST-Link
连接ST-Link SWD接口到PCB板，推荐按如下方式接线：

左侧为ST-Link接口，右侧为PCB接口：
- 3V3   <---> 3V3
- SWCLK <---> SCK
- GND   <---> GND
- SWDIO <---> SDA
- RESET <---> RST

同时，建议断开USB供电，全板仅通过ST-Link提供STM32F4所需的3.3V电源

完成连接后，将ST-Link插入电脑，打开STM32CubeProgrammer软件，选择ST-Link模式，点击Connect按钮，选择下载的固件文件，点击Program按钮，等待下载完成。

## 方案2：USB DFU
> 该方案有概率失败（受限于USB线材质量，焊接质量，以及STM32F4系列众所周知的USB bug，有概率下载失败）

按住PCB上的ST-BOOT按键 不松开，通过USB-TypeC线连接PCB板和电脑，打开STM32CubeProgrammer软件，选择USB DFU模式，点击Connect按钮，选择下载的固件文件，点击Program按钮，等待下载完成。

# 版权
bilibili 平韵の小窝 原创作品

仅限个人学习使用，严禁无授权的商业用途！
