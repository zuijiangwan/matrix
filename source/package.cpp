#include "../header/package.h"

// 构造函数
Package::Package(QByteArray originContent) : content(originContent){

}

CommandPackage::CommandPackage(QByteArray originContent) : Package::Package(originContent){
    packageNum = originContent[HEADSIZE + PACKLENSIZE]; // 包号
    commandCode = originContent[9] << 8 | originContent[10]; // 指令码
    informationLenth = originContent[11] << 8 | originContent[12]; // 额外信息长度
    information = originContent.mid(13, informationLenth); // 额外信息
}

QString CommandPackage::decode(){
    QString result("命令帧");
    return result;
}

bool CommandPackage::isData(){
    return false;
}

DataPackage::DataPackage(QByteArray originContent) : Package::Package(originContent){
    dataLength = originContent[HEADSIZE] << 8 | originContent[HEADSIZE + 1]; // 数据总长度
    rowSize = originContent[ROWPOS] << 8 | originContent[ROWPOS + 1]; // 行宽
    columnSize = originContent[COLUMNPOS] << 8 | originContent[COLUMNPOS + 1]; // 列宽
    packageNum = originContent[PACKNUMPOS] << 24 | originContent[PACKNUMPOS + 1] << 16 | originContent[PACKNUMPOS + 2] << 8 | originContent[PACKNUMPOS + 3]; // 包号
    timeStamp = originContent[TIMESTAMPPOS] << 24 | originContent[TIMESTAMPPOS + 1] << 16 | originContent[TIMESTAMPPOS + 2] << 8 | originContent[TIMESTAMPPOS + 3]; // 时间戳
    deviceType = (originContent[DEVICETYPEPOS] == FPGA); // 设备类型
    // 填充数据
    // TODO: 确认FPGA和STM32设备的区别
    int dataIndex = DATAPOS; // 数据起始位置
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

QString DataPackage::decode(){
    QString result("数据帧");
    return result;
}

bool DataPackage::isData(){
    return true;
}

ReturnPackage::ReturnPackage(QByteArray originContent) : Package::Package(originContent){
    packageNum = originContent[HEADSIZE + PACKLENSIZE]; // 包号
    status = originContent[HEADSIZE + PACKLENSIZE + 1]; // 状态码
    information = originContent.mid(HEADSIZE + PACKLENSIZE + 2, originContent.size() - HEADSIZE - PACKLENSIZE - 2); // 额外信息
}

QString ReturnPackage::decode(){
    QString result("返回帧");
    return result;
}

bool ReturnPackage::isData(){
    return false;
}