#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "serialmodule.h"
#include "bluetoothmodule.h"
#include "usbmodule.h"
#include "savefilethread.h"
#include "package.h"


class MainWindow : public QMainWindow, public Ui::MainWindow{
    Q_OBJECT

public:
    MainWindow();
    char recvdata[256]; // 接收数据缓冲区
    char *recvpos; // 接收数据缓冲区开始位置
    QReadWriteLock buflock; // 读写锁

private:
    Ui::MainWindow *ui;

    char commandHead[8] = {COMMANDHEAD}; // 命令帧包头
    int bufdatalen; // 接收缓存中有效数据的长度

    int allPackageNum; // 总帧数
    int sentPackageNum; // 已发送
    int recPackageNum; // 已接收
    int dropPackageNum; // 已丢弃

    // 模块和线程
    SerialModule *serialModule; // 串口
    BlueToothModule *blueToothModule; // 蓝牙
    USBModule *usbModule; // USB
    SaveFileThread *saveFileThread;

    void sendCommand(int commandCode, QByteArray info = ""); // 将命令填入发送框内，参数为指令码和额外信息，额外信息默认为空
    int check(QByteArray message); // 校验字算法

private slots:
    void sendData(); // 发送数据
    void receiveData(); // 接收数据
    void checkPackage(int datalen); // 检查包，输入为新收到的数据的长度

    // 菜单栏相关
    void beginSD(){sendCommand(0x00);} // 开始SD卡传输;
    void stopSD(){sendCommand(0x01);} // 停止SD卡传输;
    void initCard(){sendCommand(0x02);} // 初始化SD卡;
    void getConfig(){sendCommand(0x0c);} // 获取配置信息;
    void newCard(){sendCommand(0x0e);} // 对新卡进行初始化;

    void beginBlt(){sendCommand(0x0f);} // 开始蓝牙传输;
    void stopBlt(){sendCommand(0x10);} // 停止蓝牙传输;

    void beginSerial(){sendCommand(0x11);} // 开始串口传输;
    void stopSerial(){sendCommand(0x12);} // 停止串口传输;

    void beginUSB(){sendCommand(0x13);} // 开始USB传输;
    void stopUSB(){sendCommand(0x14);} // 停止USB传输;

    void stopAll(){sendCommand(0x15);} // 停止所有传输任务;

    void getVoltage(){sendCommand(0x16);} // 获取电池电压;
    void setRate(); // 设置传输速率;
    void setThreshold(); // 设置阈值;
    void setSize(); // 设置矩阵规模;
    void turnOn(){sendCommand(0x1a);} // 开灯;
    void turnOff(){sendCommand(0x1b);} // 关灯;

};

#endif
