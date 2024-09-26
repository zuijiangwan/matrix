#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QThread>
#include <QSerialPort>
#include <QTextEdit>
#include <QTextBrowser>
#include "serialdialog.h"

class SerialThread : public QThread{
    Q_OBJECT

private:
    SerialDialog *serialDialog;
    QSerialPort *port;
    volatile bool stopped;

signals:
    void display(QString data);

private slots:
    void receive();

public slots:
    void connectSerial();

protected:
    void run();

public:
    SerialThread();
    bool isConnected();
    bool send(QByteArray data);
};

#endif // SERIALTHREAD_H
