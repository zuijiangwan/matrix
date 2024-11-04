#include "../header/drawthread.h"

DrawThread::DrawThread(QCustomPlot *plot, QCPColorMap *colorMap) : plot(plot), colorMap(colorMap)
{
    stopped = false;
}

void DrawThread::run()
{
    while(!stopped){
        if(packqueue[nextpack-1]->isData()) // 只画数据帧
            continue;
        if(packqueuelock[nextpack-1]->tryLockForRead()){ // 尝试上锁
            
        }
    }
    stopped = false;
}