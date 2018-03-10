#include <string.h>

#include "RDCHostInfo.h"
#include "RDCClientInfo.h"

RDCClientInfo::RDCClientInfo()
{

}

RDCClientInfo::~RDCClientInfo()
{
    delete this->m_pHostInfo;
    delete this->m_pSocket;
}

RDCClientInfo *RDCClientInfo::getPeerInfo(void) const
{
    return this->m_pPeerInfo;
}

void RDCClientInfo::setPeerInfo(RDCClientInfo *pPeerInfo)
{
    this->m_pPeerInfo = pPeerInfo;
    return ;
}

RDCTcpSocket *RDCClientInfo::getSocket(void) const
{
    return this->m_pSocket;
}

void RDCClientInfo::setSocket(RDCTcpSocket *pSocket)
{
    this->m_pSocket = pSocket;
    return ;
}

RDCHostInfo *RDCClientInfo::getHostInfo(void) const
{
    return this->m_pHostInfo;
}

void RDCClientInfo::setHostInfo(const RDCHostInfo *pHostInfo)
{
    this->m_pHostInfo = new RDCHostInfo(*pHostInfo);
    return ;
}

const char *RDCClientInfo::getToken() const
{
    return this->m_pToken;
}

void RDCClientInfo::setToken(const char* token)
{
    memcpy(this->m_pToken, token, strlen(token));
    return ;
}

const char *RDCClientInfo::getPassword() const
{
    return this->m_pPassword;
}

void RDCClientInfo::setPassword(const char* password)
{
    memcpy(this->m_pPassword, password, strlen(password));
    return ;
}

const char *RDCClientInfo::getSystemVersion() const
{
    return this->m_pSystemVersion;
}

void RDCClientInfo::setSystemVersion(const char* version)
{
    memcpy(this->m_pSystemVersion, version, strlen(version));
    return ;
}

const char *RDCClientInfo::getOnlineTimeStamp() const
{
    return this->m_pOnlineTimeStamp;
}

void RDCClientInfo::setOnlineTimeStamp(const char* timestamp)
{
    memcpy(this->m_pOnlineTimeStamp, timestamp, strlen(timestamp));
    return ;
}

const char *RDCClientInfo::getCurrentStatus() const
{
    return this->m_pCurrentStatus;
}

void RDCClientInfo::setCurrentStatus(const char* status)
{
    memcpy(this->m_pCurrentStatus, status, strlen(status));
    return ;
}
