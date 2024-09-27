#include "../header/usbdialog.h"
#include <QMessageBox>

USBDialog::USBDialog(CCyUSBDevice *USBDevice, CCyUSBEndPoint *outEndPoint, CCyUSBEndPoint *inEndPoint) :
    QDialog(), USBDevice(USBDevice), outEndPoint(outEndPoint), inEndPoint(inEndPoint)
{
    setupUi(this);
    connect(SearchButton, SIGNAL(clicked()), this, SLOT(scanUSBDevice()));
    connect(OpenButton, SIGNAL(clicked()), this, SLOT(connectUSBDevice()));
    connect(CloseButton, SIGNAL(clicked()), this, SLOT(disconnectUSBDevice()));
    scanUSBDevice(); // 扫描设备
}

void USBDialog::scanUSBDevice(){ // 搜寻存在的USB设备
    PortList->clear(); // 清空设备列表
    
    if(!USBDevice)
        return;
    if(USBDevice->DeviceCount() > 0){
        for(int i = 0; i < USBDevice->DeviceCount(); i++){
            USBDevice->Open(i);
            PortList->addItem(QString(USBDevice->FriendlyName));
            USBDevice->Close();
        }
    }
}

void USBDialog::connectUSBDevice(){ // 连接USB设备
    if(PortList->count() == 0){
        return;
    }

    // 检查是否连接成功
    if(USBDevice->Open(PortList->currentRow())){
        QMessageBox::information(this, tr("information"), tr("连接成功！"));
        USBDevice->BulkInEndPt->TimeOut = 0; // 超时时间设为0
        emit connected(); // 发送信号，开始接收数据
    }
    else
        QMessageBox::critical(this, tr("error"), tr("连接失败！"));

    USBDevice->Reset();

    // 枚举端点
    for(int i = 0; i < USBDevice->EndPointCount(); i++){
        CCyUSBEndPoint *endPoint = USBDevice->EndPoints[i];
        if(endPoint->Address & 0x80) // 最高位为8，in端点
            inEndPoint = endPoint;
        else // 最高位为0，in端点
            outEndPoint = endPoint;
    }

}

void USBDialog::disconnectUSBDevice(){ // 断开USB设备
    if(!USBDevice){
        return;
    }
    USBDevice->Close();
}
