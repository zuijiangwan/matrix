#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QCoreApplication>
#include "../header/savefilethread.h"

SaveFileThread::SaveFileThread(){
    stopped = false;
    
    QDir *dir = new QDir(QDir::currentPath());
    dir->cdUp();
    filePosition = dir->path() + "/data";
    QDir defaultFold(filePosition);
    if(!defaultFold.exists())
        defaultFold.mkdir(filePosition);

    QDateTime timeCur = QDateTime::currentDateTime();
    QString defaultFileName = timeCur.toString("yyyy_MM_dd_hh_mm_ss");
    fileName = filePosition + "/" + defaultFileName + ".txt";
    file = new QFile(fileName);
    stream = new QTextStream(file);
}

void SaveFileThread::run(){
    stopped = false;
}

void SaveFileThread::setState(int state){
    this->stopped = ~state;
}

void SaveFileThread::setFilePath(){
    QDateTime timeCur = QDateTime::currentDateTime();
    QString defaultFileName = timeCur.toString("yyyy-MM-dd_hh_mm_ss");

    QString defaultFilePath = filePosition + "/" + defaultFileName;
    fileName = QFileDialog::getSaveFileName(NULL, tr("选择保存位置"), defaultFilePath, tr("Text files (*.txt)"));
    file = new QFile(fileName);
    stream = new QTextStream(file);
}

void SaveFileThread::saveContent(QString data){
    if(stopped)
        return;

    // 打开文件
    if(!file->open(QIODevice::WriteOnly | QIODevice::Append))
        return;
    
    stream->seek(file->size());
    *stream << data << "\n";
    file->close();
}
