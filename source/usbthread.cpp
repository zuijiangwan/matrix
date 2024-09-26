#include "../header/usbthread.h"

USBThread::USBThread(){
    usbDialog = NULL;
    USBDevice = new CCyUSBDevice(); // 创建USB设备对象
    USBDevice->Close();
    usbReceive = new USBReceive(USBDevice); // 创建USB接收线程
    connect(usbReceive, SIGNAL(dataReceived(QByteArray, int)), this, SLOT(dataReceived(QByteArray, int)));
    sendBuf = new unsigned char[SEND_BUF_SIZE];
    recvBuf = new unsigned char[RECV_BUF_SIZE];
}

void USBThread::connectUSB(){
    if(!usbDialog){
        usbDialog = new USBDialog(USBDevice, outEndPoint, inEndPoint);
    }
    usbDialog->show();
    usbDialog->raise();
    usbDialog->activateWindow();
}

// 发送数据，若成功发送则返回true，否则返回false
bool USBThread::sendData(QByteArray data){
    if(!USBDevice->IsOpen())
        return false;
    
    memcpy(sendBuf, data.data(), data.size());
    LONG bytesToSend = data.size();
    return USBDevice->BulkOutEndPt->XferData(sendBuf, bytesToSend);
}

bool USBThread::isConnected(){
    return USBDevice->IsOpen();
}

// 处理收到的数据
void USBThread::dataReceived(QByteArray data, int length){
    qDebug() << data;
}
