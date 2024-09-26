#ifndef SAVEFILETHREAD_H
#define SAVEFILETHREAD_H

#include <QThread>
#include <QDir>

class SaveFileThread : public QThread{
    Q_OBJECT

public:
    SaveFileThread();

protected:
    void run();

private:
    volatile bool stopped;
    QString fileName;
    QString filePosition;
    QFile *file;
    QTextStream *stream;

private slots:
    void setState(int state);
    void setFilePath();

public slots:
    void saveContent(QString data);
};

#endif 
// SAVEFILETHREAD_H
