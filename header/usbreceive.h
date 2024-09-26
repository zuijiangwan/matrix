#ifndef USBRECEIVE_H
#define USBRECEIVE_H

#include <QThread>
#include <windows.h>
#include "CyAPI.h"

#define RECV_BUF_SIZE 512

class USBReceive : public QThread{
    Q_OBJECT

private:
    volatile bool stopped;
    CCyUSBDevice *USBDevice;
    unsigned char recvBuf[RECV_BUF_SIZE];

    void receiveData();

public:
    USBReceive(CCyUSBDevice *USBDevice);

signals:
    void dataReceived(QByteArray data, int length);
};

#endif // USBRECEIVE_H
