#include <QtBluetooth>
#include <QBluetoothDeviceDiscoveryAgent> // 发现其他设备
#include <QBluetoothDeviceInfo> // 蓝牙设备信息
#include <QBluetoothSocket> // 蓝牙socket
#include <QMessageBox>
#include <QDebug>
#include "../header/bluetoothdialog.h"

BlueToothDialog::BlueToothDialog(QBluetoothSocket *socket) : socket(socket){
    // 创建对象
    setupUi(this);

    // 创建发现设备对象
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(addDeviceInfo(QBluetoothDeviceInfo))); // 发现设备信号
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished())); // 结束搜索信号

    uuid = "00001101-0000-1000-8000-00805F9B34FB"; // uuid参考上一代上位机代码随便设的

    connect(socket, SIGNAL(connected()), this, SLOT(bltConnected())); // 连接成功
    connect(ScanButton, SIGNAL(clicked()), this, SLOT(scan())); // 扫描可用蓝牙服务槽
    connect(OpenButton, SIGNAL(clicked()), this, SLOT(connectBluetooth())); // 连接蓝牙槽

    scan();
}

void BlueToothDialog::scan(){
    DeviceList->clear(); // 清空设备列表
    deviceMap.clear(); // 清空设备映射信息
    ScanButton->setText(tr("搜索中..."));
    ScanButton->setEnabled(false);

    // 发现附近的设备
    discoveryAgent->start();
}

// 添加一个设备到设备列表
void BlueToothDialog::addDeviceInfo(QBluetoothDeviceInfo info){
    if(info.coreConfigurations()){ // 如果名称不为空
        DeviceList->addItem(info.name());
        deviceMap.insert(info.name(), info);
    }
}

// 结束搜索
void BlueToothDialog::discoveryFinished(){
    ScanButton->setText(tr("搜索蓝牙"));
    ScanButton->setEnabled(true);
}

// 连接到选中的蓝牙设备
void BlueToothDialog::connectBluetooth(){
    if(DeviceList->currentItem() == 0){
        return;
    }

    QBluetoothAddress address = deviceMap.value(DeviceList->currentItem()->text()).address();
    socket->connectToService(address, QBluetoothUuid(uuid), QIODevice::ReadWrite); // 连接蓝牙，读写模式
}

// 连接成功
void BlueToothDialog::bltConnected(){
    QMessageBox::information(this, tr("information"), tr("连接成功！"));
}