#ifndef USBRECEIVE_H
#define USBRECEIVE_H

#include <QThread>
#include <QReadWriteLock>
#include <QTextBrowser>
#include <QLabel>
#include <windows.h>
#include "CyAPI.h"
#include "define.h"

extern int recPackageNum; // 已接收
extern int dropPackageNum; // 已丢弃

extern char commandHead[HEADSIZE]; // 命令帧包头
extern char dataHead[HEADSIZE]; // 数据帧包头
extern char returnHead[HEADSIZE]; // 返回帧包头

extern char recvbuf[RECVBUFSIZE]; // 接收数据缓冲区
extern QReadWriteLock recvlock; // 读写锁

class USBReceive : public QThread{
    Q_OBJECT

private:
    volatile bool stopped; // 控制线程是否中止
    CCyUSBDevice *USBDevice;
    char USBRecvBuf[USBRECVBUFSIZE]; // USB的接收数据缓冲区
    int USBRecvPos; // USB接收数据的写入位置
    QByteArray recvData; // 接收到的数据

    char check(QByteArray message); // 校验字算法

    QTextBrowser *MessageBrowser; // 消息显示框
    QLabel *recPackageLabel; // 已接收帧数
    QLabel *dropPackageLabel; // 已丢弃帧数

protected:
    void run();

public:
    USBReceive(CCyUSBDevice *USBDevice); // 构造函数
    USBReceive(CCyUSBDevice *USBDevice, QTextBrowser *MessageBrowser, QLabel *recPackageLabel, QLabel *dropPackageLabel); // 构造函数2
    QByteArray readAll(); // 读取所有数据

    QByteArray *usbLastpack;

signals:
    void usbReadReady(); // 有数据可读的信号
};

#endif // USBRECEIVE_H
