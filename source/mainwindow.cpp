#include "../header/mainwindow.h"
#include <QMessageBox>
#include <QDialog>
#include <QInputDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QCheckBox>
#include <QTime>

MainWindow::MainWindow() : QMainWindow(){
    // 绘图
    // customPlot = new QCustomPlot();
    // DrawLayout->insertWidget(0, customPlot);
    // colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
    // colorMap->data()->setSize(20, 20); // 尺寸随便初始化一个
    // colorMap->data()->setRange(QCPRange(-4, 4), QCPRange(-4, 4)); // 在键（x）和值（y）维上跨越坐标范围-4..4
    // QCPColorScale *colorScale = new QCPColorScale(customPlot); // 添加色标:
    // customPlot->plotLayout()->addElement(0, 1, colorScale); // 将其添加到主轴矩形的右侧
    // colorScale->setType(QCPAxis::atRight); // 刻度应为垂直条，刻度线/坐标轴标签右侧（实际上，右侧已经是默认值）
    // colorMap->setColorScale(colorScale); // 将颜色图与色标关联
    // colorScale->axis()->setLabel("压感数据热力图");
    // colorMap->setInterpolate(false);  // 禁止插值，显示色块
    // QCPMarginGroup *marginGroup = new QCPMarginGroup(customPlot);
    // customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    // colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    // customPlot->rescaleAxes(); // 重新缩放键（x）和值（y）轴，以便可以看到整个颜色图：
    // customPlot->replot();

    setupUi(this);

    // 初始化包队列的写锁
    for(int i = 0; i < MAXPACKNUM; i++){
        packqueuelock[i] = new QReadWriteLock();
    }

    // 初始化和显示各种包的统计值
    recPackageLabel->setText(QString::number(recPackageNum));
    dropPackageLabel->setText(QString::number(dropPackageNum));
    packageNum = 0;

    // 收发相关
    connect(SendButton, SIGNAL(clicked()), this, SLOT(sendData())); // 发送按钮
    connect(this, SIGNAL(dataReceived(int)), this, SLOT(checkHead(int))); // 收到新内容
    connect(this, SIGNAL(packReceived()), this, SLOT(checkPackage())); // 检查包buffer的内容，构造新的包

    // 串口
    serialDialog = nullptr; // 初始化串口连接窗口
    port = new QSerialPort(); // 初始化串口端口
    connect(connectSerialAct, SIGNAL(triggered()), this, SLOT(connectSerial()));
    connect(port, SIGNAL(readyRead()), this, SLOT(serialReceive()));
    // 蓝牙
    blueToothDialog = nullptr; // 初始化蓝牙连接窗口
    localDevice = new QBluetoothLocalDevice(this); // 创建本地设备对象
    if(localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) // 打开本地蓝牙
        localDevice->powerOn();
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol); // 创建套接字对象
    connect(connectBlueToothAct, SIGNAL(triggered()), this, SLOT(connectBlueTooth()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(bluetoothReceive()));
    // USB
    usbDialog = NULL; // USB连接窗口
    USBDevice = new CCyUSBDevice(); // 创建USB设备对象
    USBDevice->Close(); // 需要先关闭，否则状态会有误
    usbReceive = new USBReceive(USBDevice); // 创建USB接收线程
    connect(connectUSBAct, SIGNAL(triggered()), this, SLOT(connectUSB()));
    connect(usbReceive, SIGNAL(dataReceived(int)), this, SLOT(checkHead(int)));

    // 保存文件线程
    // saveFileThread = new SaveFileThread();
    // connect(savePositonAct, SIGNAL(triggered()), saveFileThread, SLOT(setFilePath()));
    // connect(SaveCheckBox, SIGNAL(stateChanged(int)), saveFileThread, SLOT(setState(int)));

    // 绘图线程
    // drawThread = new QThread();

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

    // 计时器，算fps和丢包率用
    lastRecPackageNum = 0;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fps()));
    timer->start(1000); // TODO：计算丢包率和fps的更新时间间隔，可修改，单位为ms
}

// 计算fps和丢包率，指收到的包的帧率
void MainWindow::fps(){ 
    fpsLabel->setText(QString::number(recPackageNum - lastRecPackageNum));
    lastRecPackageNum = recPackageNum;
    if(recPackageNum + dropPackageNum != 0)
        loseRateLabel->setText(QString::number(dropPackageNum * 100 / (recPackageNum + dropPackageNum)) + "%");
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

    // 三个IO端口都尝试发送数据，只要有一个发送成功，即视为发送成功
    if(serialSend(data) || bluetoothSend(data) || USBSend(data)){ 
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
    // 包头
    for(int i = 0; i < HEADSIZE; i++){
        data.append(commandHead[i]);
    }
    int packsize = info.size() + HEADSIZE + PACKLENSIZE + 4 + info.size(); // 计算包总长
    data.append(packsize >> 8); // 2字节包总长
    data.append(packsize & 0xff);
    data.append(packageNum);// 1字节包号
    data.append(commandCode >> 8); // 2字节指令码
    data.append(commandCode & 0xff);
    data.insert(data.size(), info); // 额外信息
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

void MainWindow::checkHead(int inputdatalen){ // 检查收到的数据内是否出现包头
    recvlock.lockForWrite(); // 上锁，退出函数前记得解锁

    static int datawait = 0; // 待接收数据长度
    static int packdatalen = 0; // 包buffer中有效数据长度
    int endpos = min(RECVSIZE + inputdatalen - 1, RECVBUFSIZE - 1); // 有效数据末尾位置
    int beginpos = datawait > 0 ? RECVSIZE : 0; // 有效数据起始位置

    if(datawait > 0){ // 检查上一次是否有剩余的数据
        int datalen = endpos - beginpos + 1;
        if(datalen >= datawait){ // 若剩余数据已经接收完
            packlock.lockForWrite();
            memcpy(packbuf + packdatalen, recvbuf + beginpos, datawait);
            packlock.unlock();
            packdatalen = 0;
            beginpos += datawait;
            datawait = 0;
            emit packReceived();
        }
        else{ // 若剩余数据未接收完
            packlock.lockForWrite();
            memcpy(packbuf + packdatalen, recvbuf + beginpos, datalen);
            packlock.unlock();
            packdatalen += datalen;
            datawait -= datalen;
            recvlock.unlock();
            return;
        }
    }
    // 检查是否出现数据帧或返回帧包头
    while(endpos - beginpos + 1 > RECVSIZE){ // 只检查带有包总长的完整包头
        if(memcmp(recvbuf + beginpos, dataHead, HEADSIZE) == 0 || memcmp(recvbuf + beginpos, returnHead, HEADSIZE) == 0){
            int packlen = recvbuf[beginpos + HEADSIZE] << 8 | recvbuf[beginpos + HEADSIZE + 1]; // 包总长
            if(endpos - beginpos + 1 >= packlen){ // 若包已经接收完
                packlock.lockForWrite();
                memcpy(packbuf + packdatalen, recvbuf + beginpos, packlen);
                packlock.unlock();
                packdatalen = 0;
                beginpos += packlen;
                emit packReceived();
            }
            else{ // 若包未接收完
                packlock.lockForWrite();
                memcpy(packbuf + packdatalen, recvbuf + beginpos, endpos - beginpos + 1);
                packlock.unlock();
                packdatalen = endpos - beginpos + 1;
                datawait = packlen - (endpos - beginpos  + 1);
                recvlock.unlock();
                return;
            }
        }
        else
            beginpos++;
    }
    // 将buffer的最后几位移到最前面
    int movesize = max(endpos - beginpos + 1, 0); // 需要移动的数据总长
    memcpy(recvbuf, recvbuf + endpos - movesize + 1, movesize);
    recvlock.unlock();
    return;
}

// 检查包的校验码是否正确
void MainWindow::checkPackage(){
    packlock.lockForRead(); // 上锁
    int packlen = packbuf[HEADSIZE] << 8 | packbuf[HEADSIZE + 1]; // 包总长
    QByteArray pack(packbuf, packlen-1); // 构造包（不带校验字）
    if(packbuf[packlen - 1] == check(pack)){ // 校验字（1字节）正确
        MessageBrowser->append("收到包");
        recPackageNum++;
        recPackageLabel->setText(QString::number(recPackageNum));

        pack.append(packbuf[packlen - 1]); // 加上校验字
        packqueuelock[nextpack]->lockForWrite(); // 上锁
        if(packqueue[nextpack]) // 若队列中有包，删除
            delete packqueue[nextpack];
        packqueue[nextpack] = new Package(pack); // 构造新的包
        packqueuelock[nextpack]->unlock(); // 解锁

        nextpack = (nextpack + 1) % MAXPACKNUM;
        emit(drawPack());
    }
    else{ // 校验字错误
        dropPackageNum++;
        dropPackageLabel->setText(QString::number(dropPackageNum));
    }
    packlock.unlock();
    return;
}

char MainWindow::check(QByteArray message){ // 校验字算法，输入的包不带校验字
    // 算法内容现在是乱写的简单版checksum
    char check = 0;
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
