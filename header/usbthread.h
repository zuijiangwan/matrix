#ifndef USBTHREAD_H
#define USBTHREAD_H

#include <QThread>
#include <windows.h>
#include "CyAPI.h"
#include "usbdialog.h"
#include "usbreceive.h"

// 为了使用CyAPI.lib
#pragma comment(lib,"SetupAPI.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

#define SEND_BUF_SIZE 256 // 发送数据缓冲区大小

class USBThread : public QThread
{
    Q_OBJECT

private:
    USBDialog *usbDialog;
    USBReceive *usbReceive;
    volatile bool stopped;
    
    CCyUSBDevice *USBDevice;	//USB设备
	CCyUSBEndPoint *outEndPoint; //USB设备输出端点
	CCyUSBEndPoint *inEndPoint;	//USB设备输入端点

private slots:
    void connectUSB();
    void dataReceived(QByteArray data, int length);

public:
    // 两个数据缓冲区必须设置成public
    unsigned char *sendBuf; // 发送数据缓冲区
    unsigned char *recvBuf; // 接收数据缓冲区

    USBThread();
    bool isConnected();
    bool sendData(QByteArray data);
};

#endif // USBTHREAD_H
