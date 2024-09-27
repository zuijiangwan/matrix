#include "../header/usbreceive.h"
#include <QDebug>

USBReceive::USBReceive(CCyUSBDevice *USBDevice) : USBDevice(USBDevice){
    stopped = false;
}

void USBReceive::run(){
    LONG length;
    while(!stopped){
        length = RECV_BUF_SIZE;
        if(USBDevice->BulkInEndPt->XferData(recvBuf, length)){
            qDebug() << "接收内容";
            for(int i = 0; i < length; i++)
                qDebug() << recvBuf[i];
            emit readReady();
        }
    }
    stopped = false;
}
