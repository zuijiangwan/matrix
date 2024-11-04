#include "../header/usbreceive.h"
#include <QDebug>

USBReceive::USBReceive(CCyUSBDevice *USBDevice) : USBDevice(USBDevice){
    stopped = false;
    USBRecvPos = 0;
}

void USBReceive::run(){
    LONG length;
    while(!stopped){
        length = RECVBUFSIZE;
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
    }
    stopped = false;
}
