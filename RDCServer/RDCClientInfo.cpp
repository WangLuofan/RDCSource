#include <string.h>

#include "RDCHostInfo.h"
#include "RDCClientInfo.h"

#define LOCK_BEGIN(m) m.lock();
#define LOCK_END(m) m.unlock()
#define LOCK(code) LOCK_BEGIN(m_pMutex) \
    do { \
        code \
    }while(0); \
    LOCK_END(this->m_pMutex)

RDCClientInfo::RDCClientInfo() : m_pPeerInfo(nullptr)
{

}

RDCClientInfo::~RDCClientInfo()
{
    delete this->m_pHostInfo;
    delete this->m_pSocket;
}

RDCClientInfo *RDCClientInfo::getPeerInfo(void)
{
    RDCClientInfo* clientInfo = nullptr;
    LOCK({
             clientInfo = this->m_pPeerInfo;
         });
    return clientInfo;
}

void RDCClientInfo::setPeerInfo(RDCClientInfo *pPeerInfo)
{
    LOCK({
            this->m_pPeerInfo = pPeerInfo;
         });
    return ;
}

RDCTcpSocket *RDCClientInfo::getSocket(void)
{
    return this->m_pSocket;
}

void RDCClientInfo::setSocket(RDCTcpSocket *pSocket)
{
    LOCK({
            this->m_pSocket = pSocket;
         });
    return ;
}

RDCHostInfo *RDCClientInfo::getHostInfo(void)
{
    return this->m_pHostInfo;
}

void RDCClientInfo::setHostInfo(const QString ipAddr, const unsigned short port, const QString hostName)
{
    LOCK({
            this->m_pHostInfo = new RDCHostInfo(ipAddr, port, hostName);
         });
    return ;
}

const QString RDCClientInfo::getToken(void) const
{
    return this->m_pToken;
}

void RDCClientInfo::setToken(const QString token)
{
    LOCK({
            this->m_pToken = token;
         });
    return ;
}

const QString RDCClientInfo::getPassword() const
{
    return this->m_pPassword;
}

void RDCClientInfo::setPassword(const QString password)
{
    LOCK({
            this->m_pPassword = password;
         });
    return ;
}

const QString RDCClientInfo::getSystemVersion() const
{
    return this->m_pSystemVersion;
}

void RDCClientInfo::setSystemVersion(const QString version)
{
    LOCK({
            this->m_pSystemVersion = version;
         });
    return ;
}

const QString RDCClientInfo::getOnlineTimeStamp() const
{
    return this->m_pOnlineTimeStamp;
}

void RDCClientInfo::setOnlineTimeStamp(const QString timestamp)
{
    LOCK({
            this->m_pOnlineTimeStamp = timestamp;
         });
    return ;
}

const QString RDCClientInfo::getCurrentStatusDesc(void) const
{
    return this->m_pCurrentStatusDesc;
}

void RDCClientInfo::setCurrentStatus(ClientStatus status)
{
    LOCK({
             this->m_pCurrentStatus = status;
             this->m_pCurrentStatusDesc = this->statusDescription(status);
         });
    return ;
}

void RDCClientInfo::setCurrentRowIndex(const int index)
{
    LOCK({
            this->m_iCurrentRowIndex = index;
         });
    return ;
}

int RDCClientInfo::getCurrentRowIndex(void) const
{
    return this->m_iCurrentRowIndex;
}

QString RDCClientInfo::statusDescription(RDCClientInfo::ClientStatus status)
{
    QString desc;

    switch(status)
    {
        case  ClientStatusOnline:
           desc = QString("在线");
           break;
        case ClientStatusOffline:
           desc = QString("离线");
           break;
        default:
           desc = QString("未知状态");
           break;
    }
    return desc;
}
