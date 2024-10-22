#ifndef PACKAGE_H
#define PACKAGE_H

#include <QVector>
#include "define.h"

// 包类别
class Package{
    private:
        QByteArray content; // 包的原始内容
        enum TYPE{COMMAND = 0, DATA, RETURN, OTHER}type; // 包的类型，命令帧、数据帧、返回帧

    public:
        Package(QByteArray originContent); // 构造函数
};

// 命令帧类别
class CommandPackage : public Package{
    private:
        int packageNum; // 包号，1字节
        int commandCode; // 指令码，2字节
        int informationLenth; // 额外信息长度，2字节
        QByteArray information; // 额外信息，0-16字节
    
    public:
        CommandPackage(QByteArray originContent); // 构造函数
};

// 数据帧类别
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

// 返回帧类别
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
