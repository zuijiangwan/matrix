#include "../header/usbmodule.h"

USBModule::USBModule(){
    usbDialog = NULL; // USB连接窗口

    USBDevice = new CCyUSBDevice(); // 创建USB设备对象
    USBDevice->Close(); // 需要先关闭，否则状态会有误

    usbReceive = new USBReceive(USBDevice); // 创建USB接收线程
}

void USBModule::connectUSB(){
    if(!usbDialog){
        usbDialog = new USBDialog(USBDevice, outEndPoint, inEndPoint);
        connect(usbDialog, SIGNAL(connected()), usbReceive, SLOT(start())); // 成功连接设备后才开始接收数据
    }
    usbDialog->show();
    usbDialog->raise();
    usbDialog->activateWindow();
}

// 发送数据，若成功发送则返回true，否则返回false
bool USBModule::sendData(QByteArray data){
    if(!USBDevice->IsOpen())
        return false;
    
    LONG bytesToSend = data.size();
    return USBDevice->BulkOutEndPt->XferData((unsigned char *)data.data(), bytesToSend);
}

bool USBModule::isConnected(){
    return USBDevice->IsOpen();
}