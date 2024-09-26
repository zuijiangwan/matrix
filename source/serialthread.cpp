#include <QMessageBox>
#include <QRegularExpression>
#include <QString>
#include "../header/serialthread.h"

void SerialThread::run(){
    stopped = false;
}

SerialThread::SerialThread(){
    serialDialog = 0;
    stopped = false;
    port = new QSerialPort();  
    connect(port, SIGNAL(readyRead()), this, SLOT(receive()));
}

void SerialThread::connectSerial(){
    if(!serialDialog){
        serialDialog = new SerialDialog(port);
    }
    serialDialog->show();
    serialDialog->raise();
    serialDialog->activateWindow();
}

bool SerialThread::send(QByteArray data){
    if(!port->isOpen()){
        return false;
    }
    port->write(data);
    return true;
}

void SerialThread::receive(){
    if(port->isOpen()){
        QByteArray data = port->readAll();
        QByteArray dataCode;
        foreach(char byte, data){
            dataCode.append(" ");
            QString temp = QString::number((uint8_t)byte, 16);
            if(temp.length() == 1)
                dataCode.append("0");
            dataCode.append(temp.toUtf8());
        }
        emit display(tr("接收：") + dataCode);
    }
}

bool SerialThread::isConnected(){
    return port->isOpen();
}
