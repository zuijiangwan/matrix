#include "../header/mainwindow.h"
#include "../header/serialmodule.h"
#include "../header/bluetoothmodule.h"
#include "../header/usbmodule.h"
#include "../header/savefilethread.h"
#include <QMessageBox>


MainWindow::MainWindow() : QMainWindow(){
    setupUi(this);

    connect(SendButton, SIGNAL(clicked()), this, SLOT(sendData()));

    // 串口线程
    serialModule = new SerialModule();
    connect(connectSerialAct, SIGNAL(triggered()), serialModule, SLOT(connectSerial()));
    // 蓝牙线程
    blueToothModule = new BlueToothModule();
    connect(connectBlueToothAct, SIGNAL(triggered()), blueToothModule, SLOT(connectBlueTooth()));
    // USB线程
    usbModule = new USBModule();
    connect(connectUSBAct, SIGNAL(triggered()), usbModule, SLOT(connectUSB()));

    // 保存文件线程
    // saveFileThread = new SaveFileThread();
    // connect(savePositonAct, SIGNAL(triggered()), saveFileThread, SLOT(setFilePath()));
    // connect(SaveCheckBox, SIGNAL(stateChanged(int)), saveFileThread, SLOT(setState(int)));

    // 菜单栏的指令
    connect(stopAct, SIGNAL(triggered()), this, SLOT(stopAll()));
    connect(beginSerialAct, SIGNAL(triggered()), this, SLOT(beginSerial()));
    connect(stopSerialAct, SIGNAL(triggered()), this, SLOT(stopSerial()));
}

void MainWindow::sendData(){
    if(MessageEdit->toPlainText().isEmpty()){ // 检查发送内容是否为空
        QMessageBox::warning(this, tr("warning"), tr("发送内容为空！"));
        return;
    }
    QByteArray data = MessageEdit->toPlainText().toLatin1(); // 待发送数据

    // 三个线程都尝试发送数据，只要有一个发送成功，即视为发送成功
    // 若发送了数据，则清空输入框并显示发送的数据
    if(serialModule->send(data) || blueToothModule->sendMsg(data) || usbModule->sendData(data)){
        MessageBrowser->append("发送：" + MessageEdit->toPlainText());
        MessageEdit->clear();
    }
    // 否则，弹出提示框
    else{
        QMessageBox::critical(this, tr("error"), tr("发送失败，请检查是否连接设备！"));
    }

    return;
}

void MainWindow::stopAll(){  
    // 8字节包头
    QByteArray data(12, 0);
    // data[0] = COMMAND_PACKAGE_HEAD_0;
    // data[1] = COMMAND_PACKAGE_HEAD_1;
    // data[2] = COMMAND_PACKAGE_HEAD_2;
    // data[3] = COMMAND_PACKAGE_HEAD_3;
    // data[4] = COMMAND_PACKAGE_HEAD_4;
    // data[5] = COMMAND_PACKAGE_HEAD_5;
    // data[6] = COMMAND_PACKAGE_HEAD_6;
    // data[7] = COMMAND_PACKAGE_HEAD_7;
    // 1字节包号
    data[8] = packageNum++; // 包号同时自增
    // 2字节指令码
    data[9] = 0x40;
    data[10] = 0x40;
    // 2字节额外信息长度
    data[11] = 0x50;
    // 1字节校验字
    data.append(check(data));

    return;
}

void MainWindow::beginSerial(){
    QString data;
    // 8字节包头
    // data = QString::number(COMMAND_PACKAGE_HEAD, 10);
    // 1字节包号
    data.append(QString::number(packageNum, 16)); 
    packageNum = (packageNum+1)%256; // 包号同时自增
    // 2字节指令码
    // 2字节额外信息长度
    // 为以上内容生成校验字
    // 把数据放到发送框里
    MessageEdit->setText(data);

    return;
}

void MainWindow::stopSerial(){
    return;
}

int MainWindow::check(QByteArray message){
    // 算法内容现在是乱写的
    int check = 0;
    for(int i = 0; i < message.size(); i++){
        check += message[i];
    }
    return check;
}   
