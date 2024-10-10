#include "../header/mainwindow.h"
#include <QMessageBox>


MainWindow::MainWindow() : QMainWindow(){
    setupUi(this);

    packageNum = 0; // 初始化包号
    connect(SendButton, SIGNAL(clicked()), this, SLOT(sendData())); // 发送按钮

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
    connect(beginSDAct, SIGNAL(triggered()), this, SLOT(beginSD())); // 开始SD卡传输
    connect(stopSDAct, SIGNAL(triggered()), this, SLOT(stopSD())); // 停止SD卡传输
    connect(initCardAct, SIGNAL(triggered()), this, SLOT(initCard())); // 初始化SD卡
    connect(getConfigAct, SIGNAL(triggered()), this, SLOT(getConfig())); // 获取配置信息
    connect(newCardAct, SIGNAL(triggered()), this, SLOT(newCard())); // 对新卡进行初始化

    connect(beginBltAct, SIGNAL(triggered()), this, SLOT(beginBlt())); // 开始蓝牙传输
    connect(stopBltAct, SIGNAL(triggered()), this, SLOT(stopBlt())); // 停止蓝牙传输
    
    connect(beginSerialAct, SIGNAL(triggered()), this, SLOT(beginSerial())); // 开始串口传输
    connect(stopSerialAct, SIGNAL(triggered()), this, SLOT(stopSerial())); // 停止串口传输

    connect(beginUSBAct, SIGNAL(triggered()), this, SLOT(beginUSB())); // 开始USB传输
    connect(stopUSBAct, SIGNAL(triggered()), this, SLOT(stopUSB())); // 停止USB传输

    connect(stopAct, SIGNAL(triggered()), this, SLOT(stopAll())); // 停止所有传输任务

    connect(getVoltageAct, SIGNAL(triggered()), this, SLOT(getVoltage())); // 获取电池电压
    connect(setRateAct, SIGNAL(triggered()), this, SLOT(setRate())); // 设置传输速率
    connect(setThresholdAct, SIGNAL(triggered()), this, SLOT(setThreshold())); // 设置阈值
    connect(setSizeAct, SIGNAL(triggered()), this, SLOT(setSize())); // 设置数据大小
    connect(turnOnAct, SIGNAL(triggered()), this, SLOT(turnOn())); // 开灯
    connect(turnOffAct, SIGNAL(triggered()), this, SLOT(turnOff())); // 关灯
}

void MainWindow::sendData(){
    if(MessageEdit->toPlainText().isEmpty()){ // 检查发送内容是否为空
        QMessageBox::warning(this, tr("warning"), tr("发送内容为空！"));
        return;
    }
    QByteArray data = QByteArray::fromHex(MessageEdit->toPlainText().toLatin1()); // 待发送数据
    // 三个线程都尝试发送数据，只要有一个发送成功，即视为发送成功
    if(serialModule->send(data) || blueToothModule->sendMsg(data) || usbModule->sendData(data)){ 
        // 若发送了数据，则清空输入框并显示发送的数据，包号自增
        MessageBrowser->append("发送：" + MessageEdit->toPlainText());
        MessageEdit->clear();
        packageNum++;
    }
    else{ 
        // 否则，弹出提示框
        QMessageBox::critical(this, tr("error"), tr("发送失败，请检查是否连接设备！"));
    }
    return;
}

void MainWindow::sendCommand(int commandCode, QByteArray info){ // 把相应命令帧填入发送框
    QByteArray data; // 待发送命令
    for(int i = 0; i < 8; i++) // 8字节包头
        data.append(commandHead[i]);
    data.append(packageNum);// 1字节包号
    data.append(commandCode >> 8); // 2字节指令码
    data.append(commandCode & 0xff);
    data.append(info.size() >> 8); // 2字节额外信息长度
    data.append(info.size() & 0xff);
    for(auto i : info) // 额外信息
        data.append(i);
    data.append(check(data)); // 1字节校验字

    // 转成String，每一个字节间插一个空格
    QString message;
    for(unsigned int i : data){
        i &= 0xff; // 只保留低8位
        if(i < 16)
            message.append("0");
        message.append(QString::number(i, 16));
        message.append(" ");
    }
    MessageEdit->setText(message);
    return;
}

int MainWindow::check(QByteArray message){
    // 算法内容现在是乱写的简单版checksum
    int check = 0;
    for(auto i : message){
        check += i;
    }
    return check;
}   
