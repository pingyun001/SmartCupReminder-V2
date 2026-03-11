# 固件说明

> 以下所有应用程序均已签名，可被bootloader识别并自动更新。 

自动更新方法：
1. 进入U盘模式，将需要更新的固件文件（update.bin）放入U盘根目录，待传输完成后，弹出U盘。
2. 为系统重新上电，等待左上角LED灯点亮。
3. 若LED为黄色，则成功识别到待更新固件，并开始尝试更新。
4. 若更新成功，则LED灯变绿色，之后系统会自动运行新固件。

- F405_CUP_MAT_Bootloader.hex：提醒喝水杯垫F405主板引导程序
- F405_CUP_MAT_V2_Debug.bin：提醒喝水杯垫F405主板应用程序，包含调试信息，修改名字为update.bin后，粘贴入U盘，可被bootloader识别并自动更新
- F405_CUP_MAT_V2_Debug.hex：提醒喝水杯垫F405主板应用程序，包含调试信息
- F405_CUP_MAT_V2_Release.bin：提醒喝水杯垫F405主板应用程序，无调试信息，修改名字为update.bin后，粘贴入U盘，可被bootloader识别并自动更新
- F405_CUP_MAT_V2_Release.hex：提醒喝水杯垫F405主板应用程序，无调试信息
- F405_Release_Merged.hex：合并固件文件，包含提醒喝水杯垫F405主板应用程序和引导程序，用于工厂批量生产
