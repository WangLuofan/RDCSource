#include "RDCServer.h"
#include "RDCTcpSocket.h"
#include "RDCClientInfo.h"
#include "RDCClientInfoList.h"
#include "RDCConfiguration.h"

RDCServer::RDCServer(QObject *parent) : QObject(parent), m_pServerSocket(new RDCTcpSocket())
{

}

void RDCServer::Start(void)
{
    this->m_pServerSocket->setEventHandler(this);
    unsigned short port = RDCConfiguration::standardConfiguration()->valueForKey("ListenPort").toInt();
    this->m_pServerSocket->ListenAtPort(port);

    return ;
}

void RDCServer::Stop(void)
{
    return ;
}

void RDCServer::onErrorOccured(RDCTcpSocket *, const char *)
{
    return ;
}

void RDCServer::onClientConnected(RDCTcpSocket* socket, const RDCHostInfo* host)
{
    RDCClientInfo* cliInfo = new RDCClientInfo();

    cliInfo->setSocket(socket);
    cliInfo->setHostInfo(host);

    return ;
}

void RDCServer::onConnectedToServer(RDCTcpSocket *)
{
    return ;
}

void RDCServer::onClientDisconnected(RDCTcpSocket *)
{
    return ;
}

void RDCServer::onTimeOutEventOccured(RDCTcpSocket *)
{
    return ;
}

void RDCServer::onMessageReceived(RDCTcpSocket *, const RDCMessage *)
{
    return ;
}
