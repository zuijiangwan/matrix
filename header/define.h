#ifndef DEFINE_H
#define DEFINE_H

// TODO：可更改模式。调试模式
#define DEBUG

// 为了使用CyAPI.lib
#pragma comment(lib,"SetupAPI.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

#define RECVBUFSIZE 4096 // 接收数据缓冲区大小
#define MAXPACKSIZE 1024 // 包缓冲区大小
#define MAXPACKNUM 20 // 暂存的包的数量
#define USBRECVBUFSIZE 1024 // USB接收数据缓冲区大小
#define HEADSIZE 4 // 包头长度
#define PACKLENSIZE 2 // 包长长度
#define RECVSIZE (HEADSIZE+PACKLENSIZE-1) // 接收buffer每次最后会留下一段数据移到最前面

// TODO：包头
#define COMMANDHEAD 0x13, 0x57, 0x9A, 0xCE
#define DATAHEAD    0x13, 0x57, 0x9A, 0xCE
#define RETURNHEAD  0x24, 0x68, 0xAC, 0xE0

// 数据帧相关数据
#define ROWPOS 7
#define COLUMNPOS 9
#define PACKNUMPOS 11
#define TIMESTAMPPOS 15
#define DEVICETYPEPOS 19
#define RECVPOS 20
#define DATAPOS 24

// 设备类型
#define FPGA  0x00
#define STM32 0xff

#endif // DEFINE_H
