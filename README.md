# 基于C++的压阻阵列型设备电脑端上位机

1. [项目简介](#项目简介)
2. [文件架构](#文件架构)
3. [环境配置](#环境配置)

## 项目简介
这里是[**江晚💖**](https://zuijiangwan.github.io)的大学生研究计划项目。

压阻阵列型智能织物是一种获取表面压力分布的传感器，
具有高分辨率、高灵敏度等特点，应用于人机交互、智慧医疗、智慧体育等领域。
压阻阵列型智能织物的输出数据是一个二维阵列，
因此数据量较大，特别是在活动识别等对数据的刷新率要求高的场景下，
对系统的数据传输带宽的要求较高。
现有的上位机程序受限于Python的运行速度，因此为了满足数据传输的带宽需求，
需要基于现有的智能织物系统重新设计基于C++语言的上位机程序。

本项目旨在设计与实现压阻阵列型设备的电脑端上位机程序，
制定与下位机的通信协议，实现与下位机的数据通信和命令交互，
实现数据的实时接收、可视化、存储功能，
支持蓝牙、串口和USB3.0的数据传输方式，
最后实现一个简单的应用场景展示。

本项目使用Qt编写图形化界面，基于多线程架构优化程序性能，
支持三种数据传输方式和FPGA、STM32两种下位机，
简化前代上位机的操作界面和使用方法，
所使用的数据传输协议为本项目特别设计。

## 文件架构
```
matrix
│   matrix.pro                  
│   README.md                   
│   resource.qrc                
├───form
│       bluetoothdialog.ui
│       mainwindow.ui
│       serialdialog.ui
│       usbdialog.ui
├───header
│   │   bluetoothdialog.h
│   │   bluetooththread.h
│   │   CyAPI.lib
│   │   mainwindow.h
│   │   package.h
│   │   savefilethread.h
│   │   serialdialog.h
│   │   serialthread.h
│   │   usbdialog.h
│   │   usbreceive.h
│   │   usbthread.h
│   └───inc
│           CyAPI.h
│           cyioctl.h
│           CyUSB30_def.h
│           usb100.h
│           usb200.h
│           UsbdStatus.h
│           VersionNo.h
├───icon
└───source
        bluetoothdialog.cpp
        bluetooththread.cpp
        main.cpp
        mainwindow.cpp
        package.cpp
        savefilethread.cpp
        serialdialog.cpp
        serialthread.cpp
        usbdialog.cpp
        usbreceive.cpp
        usbthread.cpp
```

## 环境配置
### Qt
### MSVC
### CyAPI