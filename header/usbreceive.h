#ifndef USBRECEIVE_H
#define USBRECEIVE_H

#include <QThread>
#include <windows.h>
#include "CyAPI.h"

#define RECV_BUF_SIZE 512

class USBReceive : public QThread{
    Q_OBJECT

private:
    volatile bool stopped; // 控制线程是否中止
    CCyUSBDevice *USBDevice;
    unsigned char recvBuf[RECV_BUF_SIZE];

protected:
    void run();

public:
    USBReceive(CCyUSBDevice *USBDevice); // 构造函数

signals:
    void readReady();
};

#endif // USBRECEIVE_H
