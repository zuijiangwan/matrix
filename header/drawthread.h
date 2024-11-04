#ifndef DRAWTHREAD_H
#define DRAWTHREAD_H

#include <QThread>
#include <QReadWriteLock>
#include "qcustomplot.h"
#include "package.h"
#include "define.h"

extern Package *packqueue[MAXPACKNUM]; // 收到的包队列，以循环队列的方式存储
extern int nextpack; // 包队列的队尾
extern QReadWriteLock *packqueuelock[MAXPACKNUM]; // 包buffer的读写锁

class DrawThread : public QThread
{
    Q_OBJECT

protected:
    void run();

private:
    volatile bool stopped;
    QCustomPlot *plot;
    QCPColorMap *colorMap;

public:
    DrawThread(QCustomPlot *plot, QCPColorMap *colorMap);
}

#endif // DRAWTHREAD_H
