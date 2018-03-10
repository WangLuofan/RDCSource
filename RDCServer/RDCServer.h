#ifndef RDCSERVER_H
#define RDCSERVER_H

#include "RDCTcpSocketEventHandler.h"

#include <QObject>

class RDCMessage;
class RDCHostInfo;
class RDCTcpSocket;

class RDCServer : public QObject, public RDCTcpSocketEventHandler
{
    Q_OBJECT
public:
    explicit RDCServer(QObject *parent = nullptr);

public:
    virtual void onErrorOccured(RDCTcpSocket*, const char*);
    virtual void onClientConnected(RDCTcpSocket*, const RDCHostInfo*);
    virtual void onConnectedToServer(RDCTcpSocket*);
    virtual void onClientDisconnected(RDCTcpSocket*);
    virtual void onTimeOutEventOccured(RDCTcpSocket*);
    virtual void onMessageReceived(RDCTcpSocket*, const RDCMessage*);

public slots:
    void Start(void);
    void Stop(void);

private:
    RDCTcpSocket* m_pServerSocket;
};

#endif // RDCSERVER_H
