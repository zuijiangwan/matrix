#include "../header/usbmodule.h"

USBModule::USBModule(){
    usbDialog = NULL; // USB连接窗口

    USBDevice = new CCyUSBDevice(); // 创建USB设备对象
    USBDevice->Close(); // 需要先关闭，否则状态会有误

    usbReceive = new USBReceive(USBDevice); // 创建USB接收线程
    //connect(usbReceive, SIGNAL(dataReceived(QByteArray,int)), this, SLOT(dataReceived(QByteArray,int)));

    sendBuf = new unsigned char[SEND_BUF_SIZE];
    recvBuf = new unsigned char[RECV_BUF_SIZE];
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
    
    memcpy(sendBuf, data.data(), data.size());
    LONG bytesToSend = data.size();
    return USBDevice->BulkOutEndPt->XferData(sendBuf, bytesToSend);
}

bool USBModule::isConnected(){
    return USBDevice->IsOpen();
}

// 处理收到的数据
void USBModule::dataReceived(QByteArray data, int length){
    qDebug() << data;
}

void USBModule::receiveData(){
    unsigned char recvBuf[256];
    memset(recvBuf, 0, 256);
    long length = 256;
    USBDevice->BulkInEndPt->TimeOut = 10;
    USBDevice->BulkInEndPt->XferData(recvBuf, length);
    qDebug() << "接收内容：";
    for(int i = 0; i < length; i++)
        qDebug() << recvBuf[i];
    qDebug() << "接收长度：" << length;
}
