#ifndef BLUETOOTHDIALOG_H
#define BLUETOOTHDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QtBluetooth>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>
#include "ui_bluetoothdialog.h"

class BlueToothDialog: public QDialog, public Ui::BlueToothDialog
{
    Q_OBJECT

private slots:
    void scan();
    void addDeviceInfo(QBluetoothDeviceInfo info);
    void discoveryFinished();
    void connectBluetooth();
    void disconnectBluetooth();
    void bltConnected();

private:
    QString uuid; // 蓝牙服务UUID
    QBluetoothDeviceDiscoveryAgent *discoveryAgent; //用来对周围蓝牙进行搜寻
    QBluetoothSocket *socket; //蓝牙socket
    QMap<QString, QBluetoothDeviceInfo> deviceMap; // 记录设备名到设备信息的映射

public:
    BlueToothDialog(QBluetoothSocket *socket);
};

#endif // BLUETOOTHDIALOG_H
