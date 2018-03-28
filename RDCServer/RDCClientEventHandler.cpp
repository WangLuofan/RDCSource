#include "RDCTcpSocket.h"
#include "RDCMessage.h"
#include "RDCMessagePool.h"
#include "RDCClientInfo.h"
#include "RDCClientInfoList.h"
#include "RDCHostInfo.h"

#include <QString>
#include <QTime>
#include <QDateTime>

void onClientConnectedMessageReceived(RDCTcpSocket* socket, RDCMessage* msg)
{
    //客户端发送上线信息, 从其中取出客户端主机名及系统版本信息
    int hostLen = msg->nextChar();
    QString hostName = msg->nextString(hostLen);

    int sysLen = msg->nextChar();
    QString sysver = msg->nextString(sysLen);

    //生成Token及Password
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    int token = qrand() % 1000000;
    int password = qrand() % 100000000;

    QString tokenStr = QString("%1").arg(token, 6, 10, QChar('0'));
    QString passwordStr = QString("%1").arg(password, 8, 10, QChar('0'));

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    RDCClientInfo* cliInfo = RDCClientInfoList::sharedInstance()->getClientInfo(socket);
    if(cliInfo != nullptr)
    {
        cliInfo->getHostInfo()->setHostName(hostName);
        cliInfo->setSystemVersion(sysver);
        cliInfo->setToken(tokenStr.toLatin1().constData());
        cliInfo->setPassword(passwordStr.toLatin1().constData());
        cliInfo->setOnlineTimeStamp(timestamp.toLatin1().constData());
        cliInfo->setCurrentStatus(RDCClientInfo::ClientStatusOnline);
    }

    //下发Token及Password
    MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();
    rmsg.get()->setServiceCommand(ServiceCommandACK);
    rmsg.get()->appendChar(tokenStr.length());
    rmsg.get()->appendString(tokenStr);
    rmsg.get()->appendChar(passwordStr.length());
    rmsg.get()->appendString(passwordStr);
    socket->sendMessage(rmsg.get());

    return ;
}

void onConnectionQueryMessageReceived(RDCTcpSocket* socket, RDCMessage* msg)
{
    //主控端向被控端发起连接请求, 获取Token, 添加主控端信息, 转发给被控端
    int tokenLen = msg->nextChar();
    QString token = msg->nextString(tokenLen);

    RDCClientInfo* myClientInfo = RDCClientInfoList::sharedInstance()->getClientInfo(socket);

    if(myClientInfo->getPeerInfo() != nullptr)
    {
        //被控端当前已有连接
        MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();
        rmsg.get()->setServiceCommand(ServiceCommandAlreadConnected);
        socket->sendMessage(rmsg.get());
        return ;
    }

    RDCClientInfo* otherClientInfo = RDCClientInfoList::sharedInstance()
            ->getClientInfo(token.toLatin1().constData());

    if(otherClientInfo == nullptr)
    {
        //未找到对应的信息, 回复Token Not Found
        MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();
        rmsg.get()->setServiceCommand(ServiceCommandTokenNotFound);
        socket->sendMessage(rmsg.get());

        return ;
    }

    //建立连接
    myClientInfo->setPeerInfo(otherClientInfo);
    otherClientInfo->setPeerInfo(myClientInfo);

    MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();
    rmsg.get()->setServiceCommand(ServiceCommandConnectionQuery);

    //添加HostName, IP地址
    rmsg.get()->appendChar(myClientInfo->getHostInfo()->getHostName().length());
    rmsg.get()->appendString(myClientInfo->getHostInfo()->getHostName());
    rmsg.get()->appendChar(myClientInfo->getHostInfo()->getIPAddress().length());
    rmsg.get()->appendString(myClientInfo->getHostInfo()->getIPAddress());

    //发送给被控端
    otherClientInfo->getSocket()->sendMessage(rmsg.get());

    return ;
}

void onConnectionDeniedMessageReceived(RDCTcpSocket* socket, RDCMessage* msg)
{
    //被控端拒绝建立连接
    RDCClientInfo* myClientInfo = RDCClientInfoList::sharedInstance()->getClientInfo(socket);
    RDCClientInfo* otherClientInfo = myClientInfo->getPeerInfo();

    //通知主控端, 转发原消息
    otherClientInfo->getSocket()->sendMessage(msg);

    //取消对端连接
    otherClientInfo->setPeerInfo(nullptr);
    myClientInfo->setPeerInfo(nullptr);

    return ;
}

void onPasswordVerifyMessageReceived(RDCTcpSocket* socket, RDCMessage* msg)
{
    bool shouldVerifyPassword = (bool)msg->nextChar();

    if(shouldVerifyPassword)
    {
        //被控端要求验证密码
        RDCClientInfo* myClientInfo = RDCClientInfoList::sharedInstance()->getClientInfo(socket);
        RDCClientInfo* otherClientInfo = myClientInfo->getPeerInfo();

        MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();
        rmsg.get()->setServiceCommand(ServiceCommandVerifyRequest);

        //下发密码给主控端
        rmsg.get()->appendChar(myClientInfo->getPassword().length());
        rmsg.get()->appendString(myClientInfo->getPassword());
        otherClientInfo->getSocket()->sendMessage(rmsg.get());
    }
    else
    {
        //被控端不要求验证密码, 发送密码验证通过给被控端
        MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();
        rmsg.get()->setServiceCommand(ServiceCommandVerifyComplete);
        socket->sendMessage(rmsg.get());
    }

    return ;
}

void onPasswordVerifyResultMessageReceived(RDCTcpSocket* socket, RDCMessage* msg)
{
    //主控端密码验证结果
    msg->reset();

    const ServiceCommand serviceCommand = msg->serviceCommand();

    RDCClientInfo* myClientInfo = RDCClientInfoList::sharedInstance()->getClientInfo(socket);
    RDCClientInfo* otherClientInfo = myClientInfo->getPeerInfo();

    if(serviceCommand == ServiceCommandVerifyFailed)
    {
        //主控端密码验证失败, 取消对端连接
        myClientInfo->setPeerInfo(nullptr);
        otherClientInfo->setPeerInfo(nullptr);
    }

    //转发消息给被控端
    otherClientInfo->getSocket()->sendMessage(msg);
    return ;
}

void onConnectionReadyMessageReceived(RDCTcpSocket* socket, RDCMessage* msg)
{
    //被控端已建立Udp连接, 添加IP地址，转发给主控端
    RDCClientInfo* myClientInfo = RDCClientInfoList::sharedInstance()->getClientInfo(socket);
    RDCClientInfo* otherClientInfo = myClientInfo->getPeerInfo();

    msg->appendChar(myClientInfo->getHostInfo()->getHostName().length());
    msg->appendString(myClientInfo->getHostInfo()->getIPAddress());

    otherClientInfo->getSocket()->sendMessage(msg);
    return ;
}
