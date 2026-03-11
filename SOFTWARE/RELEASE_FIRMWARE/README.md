# 固件说明

> 以下所有应用程序均已签名，可被bootloader识别并自动更新。 


## V2.1固件列表：
> 初版，实现bootloader + application分离，实现自动更新功能。

- F405_CUP_MAT_Bootloader.hex：提醒喝水杯垫F405主板引导程序
- F405_CUP_MAT_V2_Debug.bin：提醒喝水杯垫F405主板应用程序，包含调试信息，修改名字为update.bin后，粘贴入U盘，可被bootloader识别并自动更新
- F405_CUP_MAT_V2_Debug.hex：提醒喝水杯垫F405主板应用程序，包含调试信息
- F405_CUP_MAT_V2_Release.bin：提醒喝水杯垫F405主板应用程序，无调试信息，修改名字为update.bin后，粘贴入U盘，可被bootloader识别并自动更新
- F405_CUP_MAT_V2_Release.hex：提醒喝水杯垫F405主板应用程序，无调试信息
- F405_Release_Merged.hex：合并固件文件，包含提醒喝水杯垫F405主板应用程序和引导程序，用于工厂批量生产

## V2.0固件列表：
> 基础版本，无bootloader，已弃用

## 自动更新方法：
1. 寻找心仪的固件（固件列表里后缀为.bin的文件），将其改名为update.bin，并记住位置
2. 进入U盘模式，将需要更新的固件文件（update.bin）放入U盘根目录，待传输完成后，弹出U盘
3. 为系统重新上电，等待左上角LED灯点亮
4. 若LED为黄色，则成功识别到待更新固件，并开始尝试更新
5. 若更新成功，则LED灯变绿色，之后系统会自动运行新固件

## 手动更新（设备变砖了）：
如果设备更新失败，LED指示灯亮红灯，且无法进入U盘模式，则需要手动更新固件。
可参考最外层手册，3.5章节，为STM32烧录固件。
大致分为如下几步：
1. 下载并安装CubeProgrammer软件
2. 拆开喝水杯垫顶盖，露出主板，按住主板上STM32侧的BOOT按键
3. 打开CubeProgrammer软件，选择USB接口，点击Connect按钮连接杯垫
4. 选择F405_Release_Merged.hex，并点击Program按钮烧录固件
5. 等待烧录完成，断开连接，并重新上电