#include "RDCServer.h"
#include "RDCMessage.h"
#include "RDCTcpSocket.h"
#include "RDCHostInfo.h"
#include "RDCClientInfo.h"
#include "RDCClientInfoList.h"
#include "RDCConfiguration.h"
#include "RDCClientEventHandler.h"

typedef void(*MESSAGE_HANDLER)(RDCTcpSocket*, RDCMessage*);

RDCServer::RDCServer(QObject *parent) : QObject(parent), m_pServerSocket(nullptr)
{

}

RDCServer::~RDCServer()
{
    delete this->m_pServerSocket;
}

void RDCServer::Start(void)
{
    if(this->m_pServerSocket == nullptr)
    {
        this->m_pServerSocket = new RDCTcpSocket();
        this->m_pServerSocket->setEventHandler(this);
    }
    unsigned short port = RDCConfiguration::standardConfiguration()->valueForKey("ListenPort").toInt();
    this->m_pServerSocket->ListenAtPort(port);

    return ;
}

void RDCServer::Stop(void)
{
    if(this->m_pServerSocket != nullptr)
    {
        this->m_pServerSocket->Close();
        this->m_pServerSocket->ExitEventLoop();
    }
    return ;
}

void RDCServer::onErrorOccured(RDCTcpSocket *, const char *)
{
    return ;
}

void RDCServer::onClientConnected(RDCTcpSocket* socket, RDCHostInfo *host)
{
    RDCClientInfo* cliInfo = new RDCClientInfo();

    cliInfo->setSocket(socket);
    cliInfo->setHostInfo(host->getIPAddress(), host->getPort(), host->getHostName());

    RDCClientInfoList::sharedInstance()->addClientInfo(cliInfo);

    return ;
}

void RDCServer::onConnectedToServer(RDCTcpSocket *)
{
    return ;
}

void RDCServer::onClientDisconnected(RDCTcpSocket* socket)
{
    RDCClientInfo* cliInfo = RDCClientInfoList::sharedInstance()->getClientInfo(socket);

    if(cliInfo != nullptr)
    {
        const int rowIndex = cliInfo->getCurrentRowIndex();
        RDCClientInfoList::sharedInstance()->removeClientInfo(cliInfo);

        emit this->client_disconnected_signal(rowIndex);
    }

    return ;
}

void RDCServer::onTimeOutEventOccured(RDCTcpSocket *)
{
    return ;
}

void RDCServer::onMessageReceived(RDCTcpSocket* socket, RDCMessage* msg)
{
    MESSAGE_HANDLER handler = nullptr;

    const ServiceCommand serviceCommand = msg->serviceCommand();
    switch(serviceCommand)
    {
        case ServiceCommandSYN:
            handler = onClientConnectedMessageReceived;
            break;
        case ServiceCommandConnectionQuery:
            handler = onConnectionQueryMessageReceived;
            break;
        case ServiceCommandConnectionDenied:
            handler = onConnectionDeniedMessageReceived;
            break;
        case ServiceCommandVerifyRequest:
            handler = onPasswordVerifyMessageReceived;
            break;
        case ServiceCommandVerifyFailed:
        case ServiceCommandVerifyComplete:
            handler = onPasswordVerifyResultMessageReceived;
            break;
        case ServiceCommandConnectionReady:
            handler = onConnectionReadyMessageReceived;
            break;
        default:
            break;
    }

    if(handler != nullptr)
    {
        handler(socket, msg);
        if(serviceCommand == ServiceCommandSYN)
            emit this->client_connected_signal();
    }

    return ;
}
