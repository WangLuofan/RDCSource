#ifndef RDCUDPSOCKETEVENTHANDLER_H
#define RDCUDPSOCKETEVENTHANDLER_H

class RDCMessage;
class RDCHostInfo;
class RDCUdpSocket;
class RDCUdpSocketEventHandler
{
public:
    RDCUdpSocketEventHandler() {}

public:
    virtual void onScreenDataReceived(RDCUdpSocket*, RDCMessage*) = 0;
};

#endif // RDCUDPSOCKETEVENTHANDLER_H
