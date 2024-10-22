#ifndef USBMODULE_H
#define USBMODULE_H

#include <windows.h>
#include "CyAPI.h"
#include "usbdialog.h"
#include "usbreceive.h"

class USBModule : public QObject
{
    Q_OBJECT

private:
    USBDialog *usbDialog;
    
    CCyUSBDevice *USBDevice;	// USB设备
	CCyUSBEndPoint *outEndPoint; // USB设备输出端点
	CCyUSBEndPoint *inEndPoint;	// USB设备输入端点

private slots:
    void connectUSB();
    void dataReceived(QByteArray data, int length);

public:
    USBReceive *usbReceive;
    USBModule();
    bool isConnected();
    bool sendData(QByteArray data);

public slots:
    void receiveData();
};

#endif // USBMODULE_H
