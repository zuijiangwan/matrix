#include "../header/package.h"

// 构造函数，原始内容包括包头和校验码
Package::Package(QByteArray originContent) : content(originContent){
    
}

CommandPackage::CommandPackage(QByteArray originContent) : Package::Package(originContent){
    packageNum = originContent[HEADSIZE + PACKLENSIZE]; // 包号
    commandCode = originContent[9] << 8 | originContent[10]; // 指令码
    informationLenth = originContent[11] << 8 | originContent[12]; // 额外信息长度
    information = originContent.mid(13, informationLenth); // 额外信息
}

DataPackage::DataPackage(QByteArray originContent) : Package::Package(originContent){
    dataLength = originContent[8] << 8 | originContent[9]; // 数据总长度
    rowSize = originContent[10] << 8 | originContent[11]; // 行宽
    columnSize = originContent[12] << 8 | originContent[13]; // 列宽
    packageNum = originContent[14] << 24 | originContent[15] << 16 | originContent[16] << 8 | originContent[17]; // 包号
    timeStamp = originContent[18] << 24 | originContent[19] << 16 | originContent[20] << 8 | originContent[21]; // 时间戳
    deviceType = (originContent[22] == FPGA); // 设备类型
    // 第23-26字节为保留字，从第27字节开始为数据
    // 填充数据
    int dataIndex = 27; // 数据起始位置
    if(deviceType){
        // STM32，数据精度12位
        bool needToRead = true; // 是否需要读取新字节
        int nextNum; // 提前读取的下一个数字
        for(int i = 0; i < rowSize; i++){
            QVector<int> temp;
            for(int j = 0; j < columnSize; j++){
                if(needToRead){
                    temp.append(originContent[dataIndex] << 4 | originContent[dataIndex + 1] >> 4);
                    nextNum = (originContent[dataIndex + 1] & 0x0f) << 8 | originContent[dataIndex + 2];
                    dataIndex += 3;
                }
                else{
                    temp.append(nextNum);
                }
                needToRead = !needToRead;
            }
            data.append(temp);
        }
    }
    else{
        // FPGA，数据精度24位
        for(int i = 0; i < rowSize; i++){
            QVector<int> temp;
            for(int j = 0; j < columnSize; j++){
                // 每3个字节拼成一个数据
                temp.append(originContent[dataIndex] << 16 | originContent[dataIndex + 1] << 8 | originContent[dataIndex + 2]);
                dataIndex += 3;
            }
            data.append(temp);
        }
    }
}

ReturnPackage::ReturnPackage(QByteArray originContent) : Package::Package(originContent){
    packageNum = originContent[HEADSIZE + PACKLENSIZE]; // 包号
    status = originContent[HEADSIZE + PACKLENSIZE + 1]; // 状态码
    information = originContent.mid(HEADSIZE + PACKLENSIZE + 2, originContent.size() - HEADSIZE - PACKLENSIZE - 2); // 额外信息
}
