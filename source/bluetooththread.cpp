#include <QMessageBox>
#include "../header/bluetooththread.h"

void BlueToothThread::run(){
    stopped = false;
}

BlueToothThread::BlueToothThread(){
    // 创建本地设备对象
    localDevice = new QBluetoothLocalDevice(this);
    if(localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) // 打开本地蓝牙
        localDevice->powerOn();

    // 创建套接字对象
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readMsg())); // 读取数据

    blueToothDialog = 0;
    stopped = false;
}

void BlueToothThread::connectBlueTooth(){
    if(!blueToothDialog){
        blueToothDialog = new BlueToothDialog(socket);
    }
    blueToothDialog->show();
    blueToothDialog->raise();
    blueToothDialog->activateWindow();
}

// 读取数据
void BlueToothThread::readMsg(){
    QByteArray data = socket->readAll(); // 读取所有数据
    qDebug() << data;
}

// 判断是否连接
bool BlueToothThread::isConnected(){
    return socket->isOpen();
}

// 发送数据
bool BlueToothThread::sendMsg(QByteArray data){
    if(!socket->isOpen()){
        return false;
    }
    socket->write(data);
    return true;
}