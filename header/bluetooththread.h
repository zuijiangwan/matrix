#ifndef BLUETOOTHTHREAD_H
#define BLUETOOTHTHREAD_H

#include <QThread>
#include <QTextEdit>
#include <QTextBrowser>
#include <QBluetoothLocalDevice> //本机蓝牙操作
#include <QBluetoothSocket> // 蓝牙socket
#include "bluetoothdialog.h"

class BlueToothThread : public QThread{
    Q_OBJECT

private:
    QTextEdit *messageEdit;
    QTextBrowser *messageBrowser;
    volatile bool stopped;
    BlueToothDialog *blueToothDialog;

    QBluetoothLocalDevice *localDevice; //对本地设备进行操作，比如进行设备的打开，设备的关闭等等
    QBluetoothSocket *socket; //就是用来进行蓝牙配对链接和数据传输的

protected:
    void run();

public slots:
    void connectBlueTooth();

private slots:
    void readMsg();

public:
    BlueToothThread();
    bool sendMsg(QByteArray data);
    bool isConnected();
};

#endif // BLUETOOTHTHREAD_H
