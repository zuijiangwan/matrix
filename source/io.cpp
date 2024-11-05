// IO相关的函数
#include "../header/mainwindow.h"

void MainWindow::connectSerial(){ // 连接串口
    if(!serialDialog){
        serialDialog = new SerialDialog(port);
    }
    serialDialog->show();
    serialDialog->raise();
    serialDialog->activateWindow();
}

bool MainWindow::serialSend(QByteArray data){ // 串口发送数据
    if(!port->isOpen()){
        return false;
    }
    port->write(data);
    return true;
}

void MainWindow::serialReceive(){ // 串口接收数据
    if(recvlock.tryLockForWrite(0)){
        QByteArray data = port->readAll();
        #ifdef DEBUG
        qDebug() << "Received data: " << data;
        #endif
        memcpy(recvbuf + RECVSIZE, data.data(), min(data.size(), RECVBUFSIZE - RECVSIZE));
        recvlock.unlock();
        emit dataReceived(data.size());
    }
    return;
}

void MainWindow::connectBlueTooth(){
    if(!blueToothDialog){
        blueToothDialog = new BlueToothDialog(socket);
    }
    blueToothDialog->show();
    blueToothDialog->raise();
    blueToothDialog->activateWindow();
}

// 读取数据
void MainWindow::bluetoothReceive(){
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

// 发送数据
bool MainWindow::bluetoothSend(QByteArray data){
    if(!socket->isOpen()){
        return false;
    }
    socket->write(data);
    return true;
}

void MainWindow::connectUSB(){
    if(!usbDialog){
        usbDialog = new USBDialog(USBDevice, outEndPoint, inEndPoint);
        connect(usbDialog, SIGNAL(connected()), usbReceive, SLOT(start())); // 成功连接设备后才开始接收数据
    }
    usbDialog->show();
    usbDialog->raise();
    usbDialog->activateWindow();
}

// 发送数据，若成功发送则返回true，否则返回false
bool MainWindow::USBSend(QByteArray data){
    if(!USBDevice->IsOpen())
        return false;
    
    LONG bytesToSend = data.size();
    return USBDevice->BulkOutEndPt->XferData((unsigned char *)data.data(), bytesToSend);
}