#include "../header/usbreceive.h"

USBReceive::USBReceive(CCyUSBDevice *USBDevice) : USBDevice(USBDevice){
    stopped = false;
}

void USBReceive::receiveData(){
    bool flag = false;
    LONG length = RECV_BUF_SIZE;
    while(!stopped){
        flag = USBDevice->BulkInEndPt->XferData(recvBuf, length);
        if(flag) // 若成功接收数据，则发送信号
            emit dataReceived(QByteArray((char *)recvBuf), length);
    }
}
