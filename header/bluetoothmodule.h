#ifndef BLUETOOTHMODULE_H
#define BLUETOOTHMODULE_H

#include <QTextEdit>
#include <QTextBrowser>
#include <QBluetoothLocalDevice> //本机蓝牙操作
#include <QBluetoothSocket> // 蓝牙socket
#include "bluetoothdialog.h"
#include "define.h"

extern char recvbuf[RECVBUFSIZE]; // 接收数据缓冲区
extern QReadWriteLock recvlock; // 读写锁

class BlueToothModule : public QObject{
    Q_OBJECT

private:
    BlueToothDialog *blueToothDialog;

    QBluetoothLocalDevice *localDevice; //对本地设备进行操作，比如进行设备的打开，设备的关闭等等
    QBluetoothSocket *socket; //用来进行蓝牙配对链接和数据传输的

public slots:
    void connectBlueTooth();

private slots:
    void readMsg();

public:
    BlueToothModule();
    bool sendMsg(QByteArray data);
    bool isConnected();

signals:
    int dataReceived(int datalen); // 已将输入数据存入读取buffer，返回值为收到的数据的长度
};

#endif // BLUETOOTHMODULE_H
