#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "serialthread.h"
#include "bluetooththread.h"
#include "usbthread.h"
#include "savefilethread.h"

class MainWindow : public QMainWindow, public Ui::MainWindow{
    Q_OBJECT

public:
    MainWindow();

private:
    Ui::MainWindow *ui;

    unsigned int packageNum; // 命令帧包号

    // 线程
    SerialThread *serialThread;
    BlueToothThread *blueToothThread;
    USBThread *usbThread;
    SaveFileThread *saveFileThread;

    int check(QByteArray message); // 校验字算法

private slots:
    void sendData(); // 发送数据
    void stopAll(); // 停止所有传输任务
    void beginSerial(); // 开始串口传输
    void stopSerial(); // 停止串口传输
};

#endif
