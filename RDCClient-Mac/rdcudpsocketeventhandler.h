#ifndef RDCUDPSOCKETEVENTHANDLER_H
#define RDCUDPSOCKETEVENTHANDLER_H

#include "RDCMessagePool.h"

class RDCMessage;
class RDCHostInfo;
class RDCUdpSocket;
class RDCUdpSocketEventHandler
{
public:
    RDCUdpSocketEventHandler() {}

public:
    virtual void onScreenDataReceived(RDCUdpSocket*, MESSAGE_PTR) = 0;
    virtual void onScreenCommandReceived(RDCUdpSocket*, RDCMessage*) = 0;
};

#endif // RDCUDPSOCKETEVENTHANDLER_H
