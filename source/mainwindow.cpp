#include "../header/mainwindow.h"
#include <QMessageBox>
#include <QDialog>
#include <QInputDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QCheckBox>

MainWindow::MainWindow() : QMainWindow(){
    setupUi(this);

    // 初始化各种包的统计值
    allPackageNum = 0;
    sentPackageNum = 0;
    recPackageNum = 0;
    dropPackageNum = 0;
    allPackageLabel->setText(QString::number(allPackageNum));
    sentPackageLabel->setText(QString::number(sentPackageNum));
    recPackageLabel->setText(QString::number(recPackageNum));
    dropPackageLabel->setText(QString::number(dropPackageNum));

    recvpos = recvdata; // 初始化接收数据缓冲区开始位置

    connect(SendButton, SIGNAL(clicked()), this, SLOT(sendData())); // 发送按钮

    // 串口线程
    serialModule = new SerialModule(&buflock, recvpos);
    connect(connectSerialAct, SIGNAL(triggered()), serialModule, SLOT(connectSerial()));
    connect(serialModule, SIGNAL(dataReceived(int)), this, SLOT(checkPackage(int)));
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
    
    QByteArray data; // 待发送数据
    if(HexCheckBox->isChecked()) // 若勾选了16进制发送
        data = QByteArray::fromHex(MessageEdit->toPlainText().toLatin1());
    else
        data = MessageEdit->toPlainText().toLatin1();

    // 三个线程都尝试发送数据，只要有一个发送成功，即视为发送成功
    if(serialModule->send(data) || blueToothModule->sendMsg(data) || usbModule->sendData(data)){ 
        // 若发送了数据，则清空输入框并显示发送的数据，包号自增
        MessageBrowser->append("发送：" + MessageEdit->toPlainText());
        MessageEdit->clear();
        allPackageNum++;
        sentPackageNum++;
        allPackageLabel->setText(QString::number(allPackageNum));
        sentPackageLabel->setText(QString::number(sentPackageNum));
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
    data.append(sentPackageNum);// 1字节包号
    data.append(commandCode >> 8); // 2字节指令码
    data.append(commandCode & 0xff);
    data.append(info.size() >> 8); // 2字节额外信息长度
    data.append(info.size() & 0xff);
    for(auto i : info) // 额外信息
        data.append(i);
    data.append(check(data)); // 1字节校验字

    // 转成String，每一个字节间插一个空格
    QString message;
    for(int i=0; i < data.size(); i++){
        unsigned int datum = data[i] & 0xff; // 只保留低8位
        if(datum < 16)
            message.append("0");
        message.append(QString::number(datum, 16));
        message.append(" ");
    }
    MessageEdit->setText(message);
    return;
}

void MainWindow::checkPackage(int datalen){ // 检查收到的数据内是否出现包头
    int endPos = bufdatalen + datalen; // 有效数据末尾位置
    if(endPos >= RECV_BUF_SIZE){ // 若缓冲区溢出
        bufdatalen = 0;
        recvpos = recvdata;        
    }
    // 先检查是否出现完整的数据帧或返回帧包头
    // 再检查结尾处是否出现前半个包头


}

int MainWindow::check(QByteArray message){ // 校验字算法
    // 算法内容现在是乱写的简单版checksum
    int check = 0;
    for(auto i : message){
        check += i;
    }
    return check;
}

void MainWindow::setRate(){ // 设置传输速率
    int rate = QInputDialog::getInt(this, tr("设置传输速率"), tr("请输入传输速率：(Hz)"));
    QByteArray info; // 2字节速率
    info.append(rate >> 8);
    info.append(rate);
    sendCommand(0x17, info);
    return;
}

void MainWindow::setThreshold(){ // 设置阈值;
    int threshold = QInputDialog::getInt(this, tr("设置阈值"), tr("请输入阈值："));
    QByteArray info; // 2字节阈值
    info.append(threshold >> 8);
    info.append(threshold);
    sendCommand(0x18, info);
    return;
}

void MainWindow::setSize(){ // 设置矩阵规模
    // 编写一个简单输入对话框
    QDialog dialog(this);
    QFormLayout form(&dialog);
    form.addRow(new QLabel(tr("输入矩阵规模：")));
    QString value1 = QString("长：");
    QSpinBox *spinbox1 = new QSpinBox(&dialog);
    form.addRow(value1, spinbox1);
    QString value2 = QString("宽：");
    QSpinBox *spinbox2 = new QSpinBox(&dialog);
    form.addRow(value2, spinbox2);
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted){ // 当确认键按下
        QByteArray info; // 2字节矩阵规模，分别是长和宽
        info.append(spinbox1->value());
        info.append(spinbox2->value());
        sendCommand(0x19, info);
    }
    return;
}