#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "ui_mainwindow.h"
#include "define.h"
#include "serialmodule.h"
#include "bluetoothmodule.h"
#include "usbmodule.h"
#include "qcustomplot.h"
#include "savefilethread.h"
#include "package.h"

extern int allPackageNum; // 总帧数
extern int sentPackageNum; // 已发送
extern int recPackageNum; // 已接收
extern int dropPackageNum; // 已丢弃

extern char commandHead[HEADSIZE]; // 命令帧包头
extern char dataHead[HEADSIZE]; // 数据帧包头
extern char returnHead[HEADSIZE]; // 返回帧包头

extern char recvbuf[RECVBUFSIZE]; // 接收数据缓冲区
extern QReadWriteLock recvlock; // 读写锁
extern char packbuf[MAXPACKSIZE]; // 包buffer
extern QReadWriteLock packlock; // 包buffer的读写锁
extern Package *packqueue[MAXPACKNUM]; // 收到的包队列，以循环队列的方式存储
extern int nextpack; // 包队列的队首和队尾
extern QReadWriteLock *packqueuelock[MAXPACKNUM]; // 包buffer的读写锁

class MainWindow : public QMainWindow, public Ui::MainWindow{
    Q_OBJECT

public:
    MainWindow();

private:
    Ui::MainWindow *ui;

    // 绘图相关
    QCustomPlot *customPlot; // 绘图框
    QCPColorMap *colorMap; // 热力图
    QThread *drawThread; // 绘图线程

    // 模块和线程
    SerialModule *serialModule; // 串口
    BlueToothModule *blueToothModule; // 蓝牙
    USBModule *usbModule; // USB
    SaveFileThread *saveFileThread;

    // 工具函数
    void sendCommand(int commandCode, QByteArray info = ""); // 将命令填入发送框内，参数为指令码和额外信息，额外信息默认为空
    char check(QByteArray message); // 校验字算法

signals:
    void packReceived(); // 已将完整的包存入包buffer
    void drawPack(); // 给新的包画热力图

private slots:
    void sendData(); // 发送数据
    void checkHead(int datalen); // 检查包，输入为新收到的数据的长度
    void checkPackage(); // 检查包buffer的内容，构造新的包

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
