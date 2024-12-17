#include "../header/usbreceive.h"
#include <QDebug>

USBReceive::USBReceive(CCyUSBDevice *USBDevice) : USBDevice(USBDevice){
    stopped = false;
    USBRecvPos = 0;
    usbLastpack = nullptr;
}

USBReceive::USBReceive(CCyUSBDevice *USBDevice, QTextBrowser *MessageBrowser, QLabel *recPackageLabel, QLabel *dropPackageLabel) : USBDevice(USBDevice), MessageBrowser(MessageBrowser), recPackageLabel(recPackageLabel), dropPackageLabel(dropPackageLabel){
    stopped = false;
    USBRecvPos = 0;
    usbLastpack = nullptr;
}

void USBReceive::run(){
    LONG length;
    while(!stopped){
        length = USBRECVBUFSIZE;
        static QByteArray data;
        if(USBDevice->BulkInEndPt->XferData((unsigned char *)USBRecvBuf, length)){
            data.append(USBRecvBuf, length);
            int index = -1, remainsize = min(data.size(), 4); // remainsize代表data最后的一段要保留的数据的长度，一定要确保小于data.size()
            while((index = data.indexOf(dataHead, index + 1)) != -1){ // 处理数据帧
                if(index + 6 >= data.size()){
                    remainsize = data.size() - index;
                    continue;
                }
                int lenth = (data[index + 4] & 0xff) << 16 | (data[index + 5] & 0xff) << 8 | (data[index + 6] & 0xff);
                if(data.size() - index >= lenth){ // 包收完了
                    //if(usbLastpack)
                    //    delete usbLastpack;
                    usbLastpack = new QByteArray(data + index, lenth);
                    emit usbReadReady();
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
