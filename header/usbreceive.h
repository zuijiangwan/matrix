#ifndef USBRECEIVE_H
#define USBRECEIVE_H

#include <QThread>
#include <windows.h>
#include "CyAPI.h"
#include "define.h"

extern QReadWriteLock recvlock; // 读写锁

class USBReceive : public QThread{
    Q_OBJECT

private:
    volatile bool stopped; // 控制线程是否中止
    CCyUSBDevice *USBDevice;

protected:
    void run();

public:
    USBReceive(CCyUSBDevice *USBDevice); // 构造函数

signals:
    void dataReceived(int length); // 数据接收信号
};

#endif // USBRECEIVE_H
