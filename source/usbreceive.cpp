#include "../header/usbreceive.h"
#include <QDebug>

USBReceive::USBReceive(CCyUSBDevice *USBDevice) : USBDevice(USBDevice){
    stopped = false;
    USBRecvPos = 0;
}

USBReceive::USBReceive(CCyUSBDevice *USBDevice, QTextBrowser *MessageBrowser, QLabel *recPackageLabel, QLabel *dropPackageLabel) : USBDevice(USBDevice), MessageBrowser(MessageBrowser), recPackageLabel(recPackageLabel), dropPackageLabel(dropPackageLabel){
    stopped = false;
    USBRecvPos = 0;
}

void USBReceive::run(){
    LONG length;
    while(!stopped){
        length = RECVBUFSIZE;
        static QByteArray data;
        /*
        // TODO: USB还没加读写锁
        if(USBDevice->BulkInEndPt->XferData((unsigned char *)USBRecvBuf + USBRecvPos, length)){
            if(recvlock.tryLockForWrite()){
                memcpy(recvbuf + RECVSIZE, USBRecvBuf, length);
                USBRecvPos = 0;
                recvlock.unlock();
                emit dataReceived(length);
            }
            else{
                USBRecvPos += length;
            }
        }
        else if(USBRecvPos > 0){
            if(recvlock.tryLockForWrite()){
                memcpy(recvbuf + RECVSIZE, USBRecvBuf, USBRecvPos);
                USBRecvPos = 0;
                recvlock.unlock();
                emit dataReceived(length);
            }
        }
        */
        if(USBDevice->BulkInEndPt->XferData((unsigned char *)USBRecvBuf, length)){
            data.append(USBRecvBuf, length);
            qDebug() << data;
            int index = -1, remainsize = min(data.size(), 4); // remainsize代表data最后的一段要保留的数据的长度，一定要确保小于data.size()
            while((index = data.indexOf(dataHead, index + 1)) != -1){ // 处理数据帧
                if(index + 6 >= data.size()){
                    remainsize = data.size() - index;
                    continue;
                }
                int lenth = data[index + 4] << 16 | data[index + 5] << 8 | data[index + 6];
                if(data.size() - index >= lenth){ // 包收完了
                    QByteArray newpack(data + index, lenth - 1);
                    // MessageBrowser->append("USB收到数据帧");
                    if(check(newpack) == data[index + lenth - 1]){ // 校验字正确
                        recPackageNum++;
                        recPackageLabel->setText(QString::number(recPackageNum));
                    }
                    else{ // 校验字错误
                        dropPackageNum++;
                        dropPackageLabel->setText(QString::number(dropPackageNum));
                    }
                }
                else{ // 包没收完
                    remainsize = data.size() - index;
                }
            }
            // index = -1;
            // while((index = data.indexOf(returnHead, index + 1)) != -1){ // 处理返回帧
            //     if(index + 6 >= data.size()){
            //         remainsize = data.size() - index;
            //         continue;
            //     }
            //     int lenth = data[index + 4] << 16 | data[index + 5] << 8 | data[index + 6];
            //     if(data.size() - index >= lenth){ // 包收完了
            //         QByteArray newpack(data + index, lenth);
            //         // MessageBrowser->append("USB收到返回帧");
            //         if(check(newpack) == data[index + lenth - 1]){ // 校验字正确
            //             recPackageNum++;
            //             recPackageLabel->setText(QString::number(recPackageNum));
            //         }
            //         else{ // 校验字错误
            //             dropPackageNum++;
            //             dropPackageLabel->setText(QString::number(dropPackageNum));
            //         }
            //     }
            //     else{ // 包没收完
            //         remainsize = data.size() - index;
            //     }
            // }
            data = data.right(remainsize);
        }
    }
    stopped = false;
}


QByteArray USBReceive::readAll(){
    QByteArray data = recvData;
    recvData.clear();
    return data;
}

char USBReceive::check(QByteArray message){ // 校验字算法，输入的包不带校验字
    // 算法内容现在是乱写的简单版checksum
    char check = 0;
    for(auto i : message){
        check += i;
    }
    return check;
}
