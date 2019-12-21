# XC-OS
> 面向MCU的小型嵌入式图形操作系统

## 内核
> [FreeRTOS Kernel V10.2.0](https://www.freertos.org)

## UI框架
> [LittlevGL V6.0](https://github.com/littlevgl/lvgl)

## 文件系统
> [SdFat V1.1.0](https://github.com/greiman/SdFat)

## 特点
> 1.无需外扩**RAM**和**FLASH**
>
> 2.支持安装第三方APP以及驱动程序，使用Lua脚本语言编写
>
> 3.支持总线式扩展外部设备
>
> 4.系统自动识别新设备，安装驱动程序，并生成图形界面
## APPs
> 1.系统设置(主题颜色、电池信息、电源控制、下拉菜单)
>
> 2.Lua代码编辑器+调试终端
>
> 3.APP启动器
>
> 4.文件浏览器
>
> 5.文本编辑器
>
> 6.音乐播放器(支持同步显示xtrc、xlrc、lrc歌词,支持同步FFT(256点)频谱显示)
>
> 7.视频播放器(BmpVideo)
>
> 8.毁灭战士nano 3D ([doom-nano](https://github.com/daveruiz/doom-nano))
>
> 9.任务管理器
>
> 10.待续...
## 硬件配置
> 主控: STM32F405RGT6(RAM:192KB ROM:1024KB 超频216MHz)
>
> 屏幕: LCD ILI9488(3.5inch 480x320 16bit总线 70Hz刷新率)
>
> 触控: GT911
>
> 功放: PAM8403(3W)
>
> 扬声器: HUAWEI HONOR 8 扬声器总成
>
> 电池: Li-ion 3.7v 2000maH
>
> 电源管理: IP5108(5V 2A)
>
> 加速度传感器: MPU6050
>
> 外置储存器: Micro SD
>
> 通信: NRF24L01+PA / USART x2
>
> 壳体: PLA 1.75 3D打印
## 成品展示
![image](https://github.com/FASTSHIFT/XC-OS/blob/master/Images/Box_Top.jpg)
![image](https://github.com/FASTSHIFT/XC-OS/blob/master/Images/Box_Bottom.jpg)
![image](https://github.com/FASTSHIFT/XC-OS/blob/master/Images/TextEditor.jpg)
![image](https://github.com/FASTSHIFT/XC-OS/blob/master/Images/PCB_Top.jpg)
![image](https://github.com/FASTSHIFT/XC-OS/blob/master/Images/PCB_Bottom.jpg)
