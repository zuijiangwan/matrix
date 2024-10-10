#ifndef BLUETOOTHMODULE_H
#define BLUETOOTHMODULE_H

#include <QTextEdit>
#include <QTextBrowser>
#include <QBluetoothLocalDevice> //本机蓝牙操作
#include <QBluetoothSocket> // 蓝牙socket
#include "bluetoothdialog.h"

class BlueToothModule : public QObject{
    Q_OBJECT

private:
    BlueToothDialog *blueToothDialog;

    QBluetoothLocalDevice *localDevice; //对本地设备进行操作，比如进行设备的打开，设备的关闭等等
    QBluetoothSocket *socket; //就是用来进行蓝牙配对链接和数据传输的

public slots:
    void connectBlueTooth();

private slots:
    void readMsg();

public:
    BlueToothModule();
    bool sendMsg(QByteArray data);
    bool isConnected();
};

#endif // BLUETOOTHMODULE_H