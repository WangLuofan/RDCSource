#include "RDCServer.h"
#include "RDCMessage.h"
#include "RDCHostInfo.h"
#include "RDCTcpSocket.h"
#include "RDCClientInfo.h"
#include "RDCClientInfoList.h"
#include "RDCConfiguration.h"

#include <QtGlobal>
#include <QString>
#include <QTimer>
#include <QTime>

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

void RDCServer::onClientConnected(RDCTcpSocket* socket, RDCHostInfo *host)
{
    RDCClientInfo* cliInfo = new RDCClientInfo();

    cliInfo->setSocket(socket);
    cliInfo->setHostInfo(host);

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
        RDCClientInfoList::sharedInstance()->removeClientInfo(cliInfo);
    }

    return ;
}

void RDCServer::onTimeOutEventOccured(RDCTcpSocket *)
{
    return ;
}

void RDCServer::onMessageReceived(RDCTcpSocket* socket, RDCMessage* msg)
{
    const ServiceCommand serviceCommand = msg->serviceCommand();
    switch(serviceCommand)
    {
        case ServiceCommandSYN:
        {
            //客户端发送上线信息, 从其中取出客户端主机名及系统版本信息
            int hostLen = msg->nextChar();
            const char* hostname = msg->nextData(hostLen);

            int sysLen = msg->nextChar();
            const char* sysver = msg->nextData(sysLen);

            //生成Token及Password
            qsrand(QTime().secsTo(QTime::currentTime()));
            int token = qrand() % 1000000;
            int password = qrand() % 100000000;

            QString tokenStr = QString("%1").arg(token, 6, QChar('0'));
            QString passwordStr = QString("%1").arg(password, 8, QChar('0'));

            RDCClientInfo* cliInfo = RDCClientInfoList::sharedInstance()->getClientInfo(socket);
            if(cliInfo != nullptr)
            {
                cliInfo->getHostInfo()->setHostName(hostname);
                cliInfo->setSystemVersion(sysver);
                cliInfo->setToken(tokenStr.toLatin1().constData());
                cliInfo->setPassword(passwordStr.toLatin1().constData());

                QString timestamp = QTime::currentTime().toString("YYYY-MM-dd HH:mm:ss");
                cliInfo->setOnlineTimeStamp(timestamp.toLatin1().constData());
                cliInfo->setCurrentStatus("在线");
            }
        }
            break;
    }
    return ;
}
