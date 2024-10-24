#include <QMessageBox>
#include <QRegularExpression>
#include <QString>
#include "../header/serialmodule.h"

SerialModule::SerialModule(){
    serialDialog = 0;
    port = new QSerialPort();  
    connect(port, SIGNAL(readyRead()), this, SLOT(receive()));
}

void SerialModule::connectSerial(){
    if(!serialDialog){
        serialDialog = new SerialDialog(port);
    }
    serialDialog->show();
    serialDialog->raise();
    serialDialog->activateWindow();
}

bool SerialModule::send(QByteArray data){
    if(!port->isOpen()){
        return false;
    }
    port->write(data);
    return true;
}

void SerialModule::receive(){
    if(recvlock.tryLockForWrite()){
        QByteArray data = port->readAll();
        memcpy(recvbuf + RECVSIZE, data.data(), data.size());
        recvlock.unlock();
        emit dataReceived(data.size());
    }
    return;
}

bool SerialModule::isConnected(){
    return port->isOpen();
}
