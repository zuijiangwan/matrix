#ifndef SERIALMODULE_H
#define SERIALMODULE_H

#include <QSerialPort>
#include <QTextEdit>
#include <QTextBrowser>
#include "serialdialog.h"

class SerialModule : public QObject{
    Q_OBJECT

private:
    SerialDialog *serialDialog;
    QSerialPort *port;

private slots:
    void receive();

public slots:
    void connectSerial();

public:
    SerialModule();
    bool isConnected();
    bool send(QByteArray data);
};

#endif // SERIALMODULE_H
