#include <QMessageBox>
#include "../header/bluetoothmodule.h"

BlueToothModule::BlueToothModule(){
    // 创建本地设备对象
    localDevice = new QBluetoothLocalDevice(this);
    if(localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) // 打开本地蓝牙
        localDevice->powerOn();

    // 创建套接字对象
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readMsg())); // 读取数据

    blueToothDialog = 0;
}

void BlueToothModule::connectBlueTooth(){
    if(!blueToothDialog){
        blueToothDialog = new BlueToothDialog(socket);
    }
    blueToothDialog->show();
    blueToothDialog->raise();
    blueToothDialog->activateWindow();
}

// 读取数据
void BlueToothModule::readMsg(){
    if(recvlock.tryLockForWrite(0)){
        QByteArray data = socket->readAll(); // 读取所有数据
        #ifdef DEBUG
        qDebug() << "Received data: " << data;
        #endif
        memcpy(recvbuf + RECVSIZE, data.data(), min(data.size(), RECVBUFSIZE - RECVSIZE));
        recvlock.unlock();
        emit dataReceived(data.size());
    }
    return;
}

// 判断是否连接
bool BlueToothModule::isConnected(){
    return socket->isOpen();
}

// 发送数据
bool BlueToothModule::sendMsg(QByteArray data){
    if(!socket->isOpen()){
        return false;
    }
    socket->write(data);
    return true;
}