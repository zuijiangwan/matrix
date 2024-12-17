#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QSerialPort>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <windows.h>
#include "CyAPI.h"
#include "ui_mainwindow.h"
#include "define.h"
#include "serialdialog.h"
#include "bluetoothdialog.h"
#include "usbdialog.h"
#include "usbreceive.h"
#include "qcustomplot.h"
#include "savefilethread.h"
#include "package.h"

// 最大值最小值操作
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

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
extern int nextpack; // 包队列的队尾
extern QReadWriteLock *packqueuelock[MAXPACKNUM]; // 包buffer的读写锁

class MainWindow : public QMainWindow, public Ui::MainWindow{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    
    char packageNum; // 一字节命令帧包号
    
    // 计算fps和带宽用
    int packPerSec; // 每秒包数
    long long bytePerSec; // 每秒字节数

    // 模块和线程
    // SaveFileThread *saveFileThread;

    // 串口相关
    SerialDialog *serialDialog; // 串口连接窗口
    QSerialPort *port; // 串口端口
    // 蓝牙相关
    BlueToothDialog *blueToothDialog; // 蓝牙连接窗口
    QBluetoothLocalDevice *localDevice; // 对本地设备进行操作，比如进行设备的打开，设备的关闭等等
    QBluetoothSocket *socket; // 用来进行蓝牙配对链接和数据传输的代理
    // USB相关
    USBDialog *usbDialog; // USB连接窗口
    CCyUSBDevice *USBDevice; // USB设备
    USBReceive *usbReceive; // USB接收线程
	CCyUSBEndPoint *outEndPoint; // USB设备输出端点
	CCyUSBEndPoint *inEndPoint;	// USB设备输入端点

    // 保存文件相关
    QString fileName; // 文件名
    QFile *file; // 保存文件
    QTextStream *stream; // 数据流

    QByteArray lastContent; // 最新的原始包数据
    Package lastPackage; // 最新的包

    // 绘图相关
    QCustomPlot *customPlot; // 绘图框
    QCPColorMap *heatmap; // 热力图
    QByteArray *lastpack;
    int rowSize, columnSize; // 矩阵规模
    bool deviceType; // 设备类型

    // 工具函数
    void sendCommand(int commandCode, QByteArray info = ""); // 将命令填入发送框内，参数为指令码和额外信息，额外信息默认为空
    char check(QByteArray &message); // 校验字算法
    bool checkPackage(QByteArray &message); // 检查包的校验码是否正确

signals:
    void dataReceived(int datalen); // 已将输入数据存入读取buffer，返回值为收到的数据的长度
    void packReceived(); // 已将完整的包存入包buffer
    void drawPack(); // 给新的包画热力图
    void newContent(); // 有新的包内容
    void newPackage(); // 有新的包

private slots:
    void sendData(); // 发送数据
    void checkHead(int datalen); // 检查包，输入为新收到的数据的长度

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

    // 串口相关
    void connectSerial(); // 连接串口
    void serialReceive(); // 串口接收数据
    bool serialSend(QByteArray data); // 串口发送数据
    // 蓝牙相关
    void connectBlueTooth(); // 连接蓝牙
    void bluetoothReceive(); // 蓝牙接收数据
    bool bluetoothSend(QByteArray data); // 蓝牙发送数据
    // USB相关
    void connectUSB(); // 连接USB
    bool USBSend(QByteArray data); // USB发送数据
    void USBReceive(); // USB接收数据

    // 保存文件相关
    void setFilePath(); // 设置保存文件路径
    void saveContent(QByteArray *data); // 保存数据

    // 绘图相关
    void setupHeatmap(); // 设置热力图
    void refreshHeatmap(); // 刷新热力图

    void fps(); // 计算帧率
    void generatePack(); // 构造新的包
    void refreshMessage(); // 更新消息记录

};

#endif
