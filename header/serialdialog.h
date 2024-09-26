#ifndef SERIALDIALOG_H
#define SERIALDIALOG_H

#include <QDialog>
#include <QList>
#include <QSerialPort>
#include "ui_serialdialog.h"

class SerialDialog: public QDialog, public Ui::SerialDialog
{
    Q_OBJECT

public:
    SerialDialog(QSerialPort *port);

private:
    QSerialPort *port;

private slots:
    void search();
    void openSerial();
    void closeSerial();
    void updateStatus(QListWidgetItem*);
};

#endif // SERIALDIALOG_H
