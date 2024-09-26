#include <QSerialPort>
#include <QSerialPortInfo>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include "../header/serialdialog.h"

SerialDialog::SerialDialog(QSerialPort *port)
    : QDialog(), port(port)
{
    setupUi(this);

    OpenButton->setEnabled(false);
    CloseButton->setEnabled(false);

    QStringList baudList;
    baudList<<"用户设定"<<"1200"<<"2400"<<"4800"<<"9600"<<"14400"<<"19200"<<"38400"<<"57600"<<"76800";
    baudList<<"115200"<<"128000"<<"230400"<<"256000"<<"460800"<<"500000"<<"512000"<<"600000"<<"750000"<<"921600";
    BaudBox->addItems(baudList);
    BaudBox->setCurrentIndex(19);

    connect(SearchButton, SIGNAL(clicked()), this, SLOT(search()));
    connect(OpenButton, SIGNAL(clicked()), this, SLOT(openSerial()));
    connect(CloseButton, SIGNAL(clicked()), this, SLOT(closeSerial()));
    connect(PortList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateStatus(QListWidgetItem*)));

    search(); // 搜索串口
}

void SerialDialog::search(){
    PortList->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        PortList->addItem(info.portName());
}

void SerialDialog::openSerial(){
    port->setPortName(PortList->currentItem()->text());
    port->setBaudRate(BaudBox->currentText().toInt());

    if(!(port->open(QIODevice::ReadWrite))){
        QMessageBox::critical(this, tr("error"), tr("打开失败！"));
        return;
    }
    QMessageBox::information(this, tr("information"), tr("打开成功！"));
    updateStatus(PortList->currentItem());
}

void SerialDialog::closeSerial(){
    port->setPortName(PortList->currentItem()->text());
    port->clear();
    port->close();
    updateStatus(PortList->currentItem());
}

void SerialDialog::updateStatus(QListWidgetItem*){
    port->setPortName(PortList->currentItem()->text());

    OpenButton->setEnabled(!port->isOpen());
    CloseButton->setEnabled(port->isOpen());

    if(port->isOpen())
        StatusLabel->setText(tr("已打开"));
    else
        StatusLabel->setText(tr("已关闭"));
}
