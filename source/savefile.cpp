// 文件保存相关的函数
#include "../header/mainwindow.h"
#include <QDateTime>

void MainWindow::setFilePath(){
    // 关闭上一次的文件
    if(file)
        file->close();
    QString defaultFileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh_mm_ss");
    fileName = QFileDialog::getSaveFileName(NULL, tr("选择数据保存位置"), defaultFileName, tr("Text files (*.txt)"));
    // 打开要写入的文件
    file = new QFile(fileName);
    if(!file->open(QIODevice::WriteOnly | QIODevice::Append))
        return;
    // 创建数据流
    stream = new QTextStream(file);
}

void MainWindow::saveContent(QByteArray *data){
    if(!file->isOpen())
        return;
    // 写入数据
    *stream << data->toHex() << "\n";
    return;
}
