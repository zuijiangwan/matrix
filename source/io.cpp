// IO相关的函数
#include "../header/mainwindow.h"

void MainWindow::connectSerial(){ // 连接串口
    if(!serialDialog){
        serialDialog = new SerialDialog(port);
    }
    serialDialog->show();
    serialDialog->raise();
    serialDialog->activateWindow();
}

bool MainWindow::serialSend(QByteArray data){ // 串口发送数据
    if(!port->isOpen()){
        return false;
    }
    port->write(data);
    return true;
}

void MainWindow::serialReceive(){ // 串口接收数据
    static QByteArray data;
    data.append(port->readAll());
    int index = -1, remainsize = min(data.size(), 4); // remainsize代表data最后的一段要保留的数据的长度
    while((index = data.indexOf(dataHead, index + 1)) != -1){ // 处理数据帧
        if(index + 6 >= data.size()){
            remainsize = data.size() - index;
            break;
        }
        int lenth = (data[index + 4] & 0xff) << 16 | (data[index + 5] & 0xff) << 8 | (data[index + 6] & 0xff);
        if(data.size() - index >= lenth){ // 包收完了
            if(lastpack)
                delete lastpack;
            lastpack = new QByteArray(data + index, lenth);
            if(SaveCheckBox->isChecked()) // 若勾选了保存文件选项
                saveContent(lastpack); // 保存数据
            if(checkPackage(*lastpack)){ // 校验字正确
                recPackageNum++;
                recPackageLabel->setText(QString::number(recPackageNum));
                packPerSec++;
                bytePerSec += lenth;
            }
            else{ // 校验字错误
                dropPackageNum++;
                dropPackageLabel->setText(QString::number(dropPackageNum));
            }
            remainsize = data.size() - index - lenth;
        }
        else{ // 包没收完
            remainsize = data.size() - index;
        }
    }
    index = -1;
    while((index = data.indexOf(returnHead, index + 1)) != -1){ // 处理返回帧
        if(index + 6 >= data.size()){
            remainsize = data.size() - index;
            break;
        }
        int lenth = (data[index + 4] & 0xff) << 16 | (data[index + 5] & 0xff) << 8 | (data[index + 6] & 0xff);
        if(data.size() - index >= lenth){ // 包收完了
            if(lastpack)
                delete lastpack;
            lastpack = new QByteArray(data + index, lenth);
            if(checkPackage(*lastpack)){ // 校验字正确
                recPackageNum++;
                recPackageLabel->setText(QString::number(recPackageNum));
                packPerSec++;
                MessageBrowser->append(tr("收到返回帧"));
            }
            else{ // 校验字错误
                dropPackageNum++;
                dropPackageLabel->setText(QString::number(dropPackageNum));
                bytePerSec += lenth;
            }
            remainsize = data.size() - index - lenth;
        }
        else{ // 包没收完
            remainsize = data.size() - index;
        }
    }
    data = data.right(remainsize);
    return;
}

void MainWindow::connectBlueTooth(){
    if(!blueToothDialog){
        blueToothDialog = new BlueToothDialog(socket);
    }
    blueToothDialog->show();
    blueToothDialog->raise();
    blueToothDialog->activateWindow();
}

// 蓝牙读取数据
void MainWindow::bluetoothReceive(){
    static QByteArray data;
    data.append(socket->readAll());
    int index = -1, remainsize = min(data.size(), 4); // remainsize代表data最后的一段要保留的数据的长度
    while((index = data.indexOf(dataHead, index + 1)) != -1){ // 处理数据帧
        if(index + 6 >= data.size()){
            remainsize = data.size() - index;
            break;
        }
        int lenth = (data[index + 4] & 0xff) << 16 | (data[index + 5] & 0xff) << 8 | (data[index + 6] & 0xff);
        if(data.size() - index >= lenth){ // 包收完了
            lastpack = new QByteArray(data + index, lenth);
            saveContent(lastpack);
            if(checkPackage(*lastpack)){ // 校验字正确
                recPackageNum++;
                recPackageLabel->setText(QString::number(recPackageNum));
                packPerSec++;
                bytePerSec += lenth;
                if(SaveCheckBox->isChecked()) // 若勾选了保存文件选项
                    saveContent(lastpack); // 保存数据
            }
            else{ // 校验字错误
                dropPackageNum++;
                dropPackageLabel->setText(QString::number(dropPackageNum));
            }
            remainsize = data.size() - index - lenth;
        }
        else{ // 包没收完
            remainsize = data.size() - index;
        }
    }
    index = -1;
    while((index = data.indexOf(returnHead, index + 1)) != -1){ // 处理返回帧
        if(index + 6 >= data.size()){
            remainsize = data.size() - index;
            break;
        }
        int lenth = (data[index + 4] & 0xff) << 16 | (data[index + 5] & 0xff) << 8 | (data[index + 6] & 0xff);
        if(data.size() - index >= lenth){ // 包收完了
            lastpack = new QByteArray(data + index, lenth);
            if(checkPackage(*lastpack)){ // 校验字正确
                recPackageNum++;
                recPackageLabel->setText(QString::number(recPackageNum));
                packPerSec++;
            }
            else{ // 校验字错误
                dropPackageNum++;
                dropPackageLabel->setText(QString::number(dropPackageNum));
                bytePerSec += lenth;
            }
            remainsize = data.size() - index - lenth;
        }
        else{ // 包没收完
            remainsize = data.size() - index;
        }
    }
    data = data.right(remainsize);
    return;
}

// 发送数据
bool MainWindow::bluetoothSend(QByteArray data){
    if(!socket->isOpen()){
        return false;
    }
    socket->write(data);
    return true;
}

void MainWindow::connectUSB(){
    if(!usbDialog){
        usbDialog = new USBDialog(USBDevice, outEndPoint, inEndPoint);
        connect(usbDialog, SIGNAL(connected()), usbReceive, SLOT(start())); // 成功连接设备后才开始接收数据
    }
    usbDialog->show();
    usbDialog->raise();
    usbDialog->activateWindow();
}

// 发送数据，若成功发送则返回true，否则返回false
bool MainWindow::USBSend(QByteArray data){
    if(!USBDevice->IsOpen())
        return false;
    
    LONG bytesToSend = data.size();
    return USBDevice->BulkOutEndPt->XferData((unsigned char *)data.data(), bytesToSend);
}

// USB读取数据
void MainWindow::USBReceive(){
    if(SaveCheckBox->isChecked()) // 若勾选了保存文件选项
        saveContent(usbReceive->usbLastpack); // 保存数据
    if(checkPackage(*usbReceive->usbLastpack)){ // 校验字正确
        recPackageNum++;
        recPackageLabel->setText(QString::number(recPackageNum));
        packPerSec++;
        bytePerSec += usbReceive->usbLastpack->size();
    }
    else{ // 校验字错误
        dropPackageNum++;
        dropPackageLabel->setText(QString::number(dropPackageNum));
    }
    return;
}
