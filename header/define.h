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

// 设备类型
#define FPGA  0x00
#define STM32 0xff

#endif // DEFINE_H