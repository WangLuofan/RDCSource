#ifndef RDCTCPSOCKETEVENTHANDLER_H
#define RDCTCPSOCKETEVENTHANDLER_H

class RDCMessage;
class RDCHostInfo;
class RDCTcpSocket;
class RDCTcpSocketEventHandler
{
public:
    RDCTcpSocketEventHandler() {}

public:
    virtual void onErrorOccured(RDCTcpSocket*, const char*) = 0;
    virtual void onClientConnected(RDCTcpSocket*, const RDCHostInfo* const) = 0;
    virtual void onConnectedToServer(RDCTcpSocket*) = 0;
    virtual void onClientDisconnected(RDCTcpSocket*) = 0;
    virtual void onTimeOutEventOccured(RDCTcpSocket*) = 0;
    virtual void onMessageReceived(RDCTcpSocket*, const RDCMessage* const) = 0;
};

#endif // RDCTCPSOCKETEVENTHANDLER_H
