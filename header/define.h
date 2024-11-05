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
#define USBRECVBUFSIZE 4096 // USB接收数据缓冲区大小
#define HEADSIZE 4 // 包头长度
#define PACKLENSIZE 2 // 包长长度
#define RECVSIZE (HEADSIZE+PACKLENSIZE-1) // 接收buffer每次最后会留下一段数据移到最前面

// TODO：包头
#define COMMANDHEAD 0x55, 0x55, 0x55, 0x55
#define DATAHEAD    0x13, 0x57, 0x9A, 0xCE
#define RETURNHEAD  0x5A, 0x5A, 0x5A, 0x5A

// 数据帧相关数据
#define ROWPOS 6
#define COLUMNPOS 8
#define PACKNUMPOS 10
#define TIMESTAMPPOS 14
#define DEVICETYPEPOS 18
#define RECVPOS 19
#define DATAPOS 23

// 设备类型
#define FPGA  0x00
#define STM32 0xff

#endif // DEFINE_H
