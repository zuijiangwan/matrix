#ifndef USBRECEIVE_H
#define USBRECEIVE_H

#include <QThread>
#include <QReadWriteLock>
#include <windows.h>
#include "CyAPI.h"
#include "define.h"

extern char recvbuf[RECVBUFSIZE]; // 接收数据缓冲区
extern QReadWriteLock recvlock; // 读写锁

class USBReceive : public QThread{
    Q_OBJECT

private:
    volatile bool stopped; // 控制线程是否中止
    CCyUSBDevice *USBDevice;
    char USBRecvBuf[USBRECVBUFSIZE]; // USB的接收数据缓冲区
    int USBRecvPos; // USB接收数据的写入位置

protected:
    void run();

public:
    USBReceive(CCyUSBDevice *USBDevice); // 构造函数

signals:
    void dataReceived(int length); // 数据接收信号
};

#endif // USBRECEIVE_H
