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
    //customPlot = new QCustomPlot();
    //heatmapLayout->addWidget(customPlot);
    //heatmap = nullptr;

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
    //connect(this, SIGNAL(packReceived()), this, SLOT(checkPackage())); // 检查包buffer的内容，构造新的包
    connect(this, SIGNAL(newContent()), this, SLOT(generatePack())); // 构造新的包
    connect(this, SIGNAL(newPackage()), this, SLOT(refreshMessage())); // 更新消息记录

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
    usbReceive = new class USBReceive(USBDevice, MessageBrowser, recPackageLabel, dropPackageLabel); // 创建USB接收线程
    connect(connectUSBAct, SIGNAL(triggered()), this, SLOT(connectUSB()));
    connect(usbReceive, SIGNAL(readReady()), this, SLOT(USBReceive()));

    // 保存文件
    connect(savePositonAct, SIGNAL(triggered()), this, SLOT(setFilePath()));

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

    // fps计时器，算fps和带宽用
    packPerSec = 0;
    bytePerSec = 0;
    QTimer *fpstimer = new QTimer(this);
    connect(fpstimer, SIGNAL(timeout()), this, SLOT(fps()));
    fpstimer->start(1000); // TODO：计算丢包率和fps的更新时间间隔，可修改，单位为ms

    // 绘图计时器，更新热力图用

    setFilePath(); // 设置文件路径
}

MainWindow::~MainWindow(){
    // 关闭文件
    if(file)
        file->close();
}

// 计算fps和带宽，指收到的包的帧率
void MainWindow::fps(){ 
    fpsLabel->setText(QString::number(packPerSec) + "/s");
    packPerSec = 0;

    long long displayBytePerSec = bytePerSec;
    bytePerSec = 0;
    if(displayBytePerSec > (1 << 20)){ // 大于1MB
        rateLabel->setText(QString::number(displayBytePerSec >> 20) + "MB/s");
    }
    else if(displayBytePerSec > (1 << 10)){ // 大于1KB
        rateLabel->setText(QString::number(displayBytePerSec >> 10) + "KB/s");
    }
    else
        rateLabel->setText(QString::number(displayBytePerSec) + "B/s");
}

// 构造新的包
void MainWindow::generatePack(){
    // 检查校验字
    if(!checkPackage(lastContent)){ // 校验字错误
        dropPackageNum++;
        dropPackageLabel->setText(QString::number(dropPackageNum));
    }
    else{ // 校验字正确
        recPackageNum++;
        recPackageLabel->setText(QString::number(recPackageNum));
        /*packPerSec++;
        bytePerSec += lastContent.size();
        
        if(SaveCheckBox->isChecked()) // 若勾选了保存文件选项
            saveContent(lastContent); // 保存数据

        if(lastContent.startsWith(commandHead)) // 若是命令帧
            lastPackage = CommandPackage(lastContent);
        else if(lastContent.startsWith(dataHead)) // 若是数据帧
            lastPackage = DataPackage(lastContent);

        emit newPackage();*/
    }
}

// 更新消息记录
void MainWindow::refreshMessage(){
    if(ignoreDataCheckBox->isChecked() && lastPackage.isData()) // 若勾选了忽略数据帧选项
        return;
    if(originContentCheckBox->isChecked())
        MessageBrowser->append("收到：" + lastContent.toHex());
    else
        MessageBrowser->append("收到：" + lastPackage.decode());
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

void MainWindow::setupHeatmap(){
    QCPColorMap *heatmap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);  // 构造一个颜色图
    rowSize = *lastpack[ROWPOS] << 8 | *lastpack[ROWPOS + 1]; // 行宽
    columnSize = *lastpack[COLUMNPOS] << 8 | *lastpack[COLUMNPOS + 1]; // 列宽
    deviceType = (*lastpack[DEVICETYPEPOS] == FPGA); // 设备类型
    heatmap->data()->setSize(rowSize, columnSize);   // 设置颜色图数据维度，其内部维护着一个一维数组（一般表现为二维数组），这里可以理解为有多少个小方块
    heatmap->data()->setRange(QCPRange(0.5, rowSize - 0.5), QCPRange(0.5, columnSize - 0.5));  // 颜色图在x、y轴上的范围

    // 设置轴的显示，这里使用文字轴
    QSharedPointer<QCPAxisTickerText> xTicker(new QCPAxisTickerText);
    QSharedPointer<QCPAxisTickerText> yTicker(new QCPAxisTickerText);
    xTicker->setSubTickCount(1);
    yTicker->setSubTickCount(1);
    customPlot->xAxis->setTicker(xTicker);
    customPlot->yAxis->setTicker(yTicker);
    customPlot->xAxis->grid()->setPen(Qt::NoPen);
    customPlot->yAxis->grid()->setPen(Qt::NoPen);
    customPlot->xAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->grid()->setSubGridVisible(true);
    customPlot->xAxis->setSubTicks(true);
    customPlot->yAxis->setSubTicks(true);
    customPlot->xAxis->setTickLength(0);
    customPlot->yAxis->setTickLength(0);
    customPlot->xAxis->setSubTickLength(6);
    customPlot->yAxis->setSubTickLength(6);
    customPlot->xAxis->setRange(0, columnSize);
    customPlot->yAxis->setRange(0, rowSize);

    QCPColorScale *colorScale = new QCPColorScale(customPlot);  // 构造一个色条
    colorScale->setType(QCPAxis::atBottom);   // 水平显示
    customPlot->plotLayout()->addElement(1, 0, colorScale); // 在颜色图下面显示
    heatmap->setColorScale(colorScale); 
    QCPColorGradient gradient;  // 色条使用的颜色渐变
    gradient.setColorStopAt(0.0, QColor("#f6efa6"));   // 设置色条开始时的颜色
    gradient.setColorStopAt(1.0, QColor("#bf444c"));  // 设置色条结束时的颜色
    heatmap->setGradient(gradient);
//    colorMap->rescaleDataRange();        // 自动计算数据范围，数据范围决定了哪些数据值映射到QCPColorGradient的颜色渐变当中
    heatmap->setDataRange(QCPRange(0, 10));     // 为了保持与echart的例子一致，我们这里手动设置数据范围
    heatmap->setInterpolate(false);         // 为了显示小方块，我们禁用插值

    // 保持色条与轴矩形边距一致
    QCPMarginGroup *marginGroup = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    colorScale->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

}

void MainWindow::refreshHeatmap(){
    if(!heatmap)
        setupHeatmap();

    int dataIndex = DATAPOS; // 数据起始位置
    int num;
    if(deviceType){
        // STM32，数据精度12位
        bool needToRead = true; // 是否需要读取新字节
        int nextnum; // 提前读取的下一个数字
        for(int i = 0; i < rowSize; i++){
            for(int j = 0; j < columnSize; j++){
                if(needToRead){
                    num = *lastpack[dataIndex] << 4 | *lastpack[dataIndex + 1] >> 4;
                    nextnum = (*lastpack[dataIndex + 1] & 0x0f) << 8 | *lastpack[dataIndex + 2];
                    dataIndex += 3;
                    if(heatmap->data()->alpha(i, j))
                        heatmap->data()->setCell(i, j, num);
                }
                else{
                    if(heatmap->data()->alpha(i, j))
                        heatmap->data()->setCell(i, j, nextnum);
                }
                needToRead = !needToRead;
            }
        }
    }
    else{
        // FPGA，数据精度24位
        for(int i = 0; i < rowSize; i++){
            for(int j = 0; j < columnSize; j++){
                // 每3个字节拼成一个数据
                num = *lastpack[dataIndex] << 16 | *lastpack[dataIndex + 1] << 8 | *lastpack[dataIndex + 2];
                dataIndex += 3;
                if(heatmap->data()->alpha(i, j))
                        heatmap->data()->setCell(i, j, num);
            }
        }
    }
}

char MainWindow::check(QByteArray &message){ // 校验字算法
    // TODO: 算法现在是乱写的简单版checksum
    char check = 0;
    for(auto i : message){
        check += i;
    }
    return check;
}

bool MainWindow::checkPackage(QByteArray &message){ // 检查包的校验字是否正确
    // TODO: 算法现在是乱写的简单版checksum
    char check = 0;
    for(int i = 0; i < message.size() - 1; i++){
        check += message[i];
    }
    return check == message[message.size() - 1];
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
