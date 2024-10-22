#include "../header/usbreceive.h"
#include <QDebug>

USBReceive::USBReceive(CCyUSBDevice *USBDevice) : USBDevice(USBDevice){
    stopped = false;
}

void USBReceive::run(){
    LONG length;
    while(!stopped){
        length = RECVBUFSIZE;
        if(USBDevice->BulkInEndPt->XferData((unsigned char *)recvbuf + RECVSIZE, length)){
            emit dataReceived(length);
        }
    }
    stopped = false;
}
