#include <QApplication>
#include <QFile>
#include "../header/mainwindow.h"

// 全局变量
char commandHead[HEADSIZE] = {COMMANDHEAD}; // 命令帧包头
char dataHead[HEADSIZE] = {DATAHEAD}; // 数据帧包头
char returnHead[HEADSIZE] = {RETURNHEAD}; // 返回帧包头

char recvbuf[RECVBUFSIZE]; // 接收数据缓冲区
QReadWriteLock recvlock; // 读写锁
char packbuf[MAXPACKSIZE]; // 包buffer
QReadWriteLock packlock; // 包buffer的读写锁
Package *packqueue[MAXPACKNUM] = {nullptr}; // 收到的包队列，以循环队列的方式存储
int lastpack = 0; // 包队列的队首和队尾
QReadWriteLock packqueuelock[MAXPACKNUM]; // 包buffer的读写锁

int allPackageNum = 0; // 总帧数
int sentPackageNum = 0; // 已发送
int recPackageNum = 0; // 已接收
int dropPackageNum = 0; // 已丢弃

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
