#ifndef PACKAGE_H
#define PACKAGE_H

#include <QVector>

// 三种包的包头，长度均为8字节（内容乱定的）
#define COMMANDHEAD 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
#define DATAHEAD    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
#define RETURNHEAD  0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A
// 设备类型
#define FPGA  0x00
#define STM32 0xff

class Package{
    private:
        QByteArray content; // 包的原始内容
        enum TYPE{COMMAND = 0, DATA, RETURN, OTHER}type; // 包的类型，命令帧、数据帧、返回帧

    public:
        Package(); // 构造函数
        Package(QByteArray originContent); // 构造函数
};

class CommandPackage : public Package{
    private:
        int packageNum; // 包号，1字节
        int commandCode; // 指令码，2字节
        int informationLenth; // 额外信息长度，2字节
        QByteArray information; // 额外信息，0-16字节

        void fillContent(); // 填充content
    
    public:
        CommandPackage(QByteArray originContent); // 构造函数
        CommandPackage(int packageNum, int commandCode, QByteArray info = NULL); // 构造函数，需要的信息为：指令码，额外信息（默认为空）
};

class DataPackage : public Package{
    private:
        int packageNum; // 包号
        int timeStamp; // 时间戳
        bool deviceType; // true代表FPGA，数据精度12位，false代表STM32，数据精度24位
        int dataLength; // 数据长度
        int rowSize, columnSize; // 行宽和列宽
        QVector<QVector <int>> data; // 数据

    public:
        DataPackage(QByteArray originContent); // 构造函数
};

class ReturnPackage : public Package{
    private:
        int packageNum; // 包号
        int status; // 状态码
        int informationLenth; // 额外信息长度
        QByteArray information; // 额外信息

    public:
        ReturnPackage(QByteArray originContent); // 构造函数
};

#endif // PACKAGE_H
