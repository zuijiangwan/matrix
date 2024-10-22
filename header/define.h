#ifndef DEFINE_H
#define DEFINE_H

#include <QReadWriteLock>

// 为了使用CyAPI.lib
#pragma comment(lib,"SetupAPI.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

#define RECVBUFSIZE 256 // 接收数据缓冲区大小
#define MAXPACKSIZE 256 // 包缓冲区大小
#define MAXPACKNUM 20 // 暂存的包的数量
#define HEADSIZE 8 // 包头长度
#define PACKLENSIZE 2 // 包长长度
#define RECVSIZE (HEADSIZE+PACKLENSIZE-1) // 接收buffer每次最后会留下一段数据移到最前面

// TODO：包头
#define COMMANDHEAD 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
#define DATAHEAD    0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58
#define RETURNHEAD  0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A
char commandHead[HEADSIZE] = {COMMANDHEAD}; // 命令帧包头
char dataHead[HEADSIZE] = {DATAHEAD}; // 数据帧包头
char returnHead[HEADSIZE] = {RETURNHEAD}; // 返回帧包头
// 设备类型
#define FPGA  0x00
#define STM32 0xff

class Package;

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

#endif