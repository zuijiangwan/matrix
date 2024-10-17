#ifndef SERIALMODULE_H
#define SERIALMODULE_H

#include <QSerialPort>
#include <QTextEdit>
#include <QTextBrowser>
#include <QReadWriteLock>
#include "serialdialog.h"

class SerialModule : public QObject{
    Q_OBJECT

private:
    SerialDialog *serialDialog;
    QSerialPort *port;
    QReadWriteLock *buflock;
    char *recvdata;

signals:
    int dataReceived(int datalen); // 已将输入数据存入读取buffer，返回值为收到的数据的长度

private slots:
    void receive();

public slots:
    void connectSerial();

public:
    SerialModule(QReadWriteLock *buflock, char *recvdata);
    bool isConnected();
    bool send(QByteArray data);
};

#endif // SERIALMODULE_H
