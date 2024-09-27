#ifndef USBDIALOG_H
#define USBDIALOG_H

#include <QDialog>
#include <windows.h>
#include "ui_usbdialog.h"
#include "CyAPI.h"

class USBDialog: public QDialog, public Ui::USBDialog
{
    Q_OBJECT

public:
    USBDialog(CCyUSBDevice *USBDevice, CCyUSBEndPoint *outEndPoint, CCyUSBEndPoint *inEndPoint); // 构造函数

signals:
    void connected(); // 成功连接USB设备

private:
    CCyUSBDevice *USBDevice; // USB设备，用来连接USB
    CCyUSBEndPoint *outEndPoint; // USB设备输出端点
    CCyUSBEndPoint *inEndPoint; // USB设备输入端点

private slots:
    void scanUSBDevice(); // 搜寻存在的USB设备
    void connectUSBDevice(); // 连接USB设备
    void disconnectUSBDevice(); // 断开USB设备
};

#endif // USBDIALOG_H
