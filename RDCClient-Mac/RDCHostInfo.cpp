#include <string.h>
#include "RDCHostInfo.h"

#include <arpa/inet.h>

RDCHostInfo::RDCHostInfo() :
    m_usPort(0), m_pIPAddress(QString()), m_pHostName(QString())
{
}

RDCHostInfo::RDCHostInfo(struct sockaddr* addr) :
    m_pHostName(QString())
{
    this->m_usPort = ntohs(((struct sockaddr_in*)addr)->sin_port);

    const char* ipaddr = inet_ntoa(((struct sockaddr_in*)addr)->sin_addr);
    this->m_pIPAddress = QString(ipaddr);
}

RDCHostInfo::RDCHostInfo(const QString ipAddr, const unsigned short port, const QString hostName) :
    m_pIPAddress(ipAddr), m_usPort(port), m_pHostName(hostName)
{
    return ;
}

RDCHostInfo::RDCHostInfo(const RDCHostInfo& hostinfo)
{
    this->setIPAddress(hostinfo.getIPAddress());
    this->setHostName(hostinfo.getHostName());
    this->setPort(hostinfo.getPort());
}

unsigned short RDCHostInfo::getPort(void) const
{
    return this->m_usPort;
}

void RDCHostInfo::setPort(unsigned short usPort)
{
    this->m_usPort = usPort;
    return ;
}

const QString RDCHostInfo::getIPAddress(void) const
{
    return this->m_pIPAddress;
}

void RDCHostInfo::setIPAddress(QString ipaddr)
{
    this->m_pIPAddress = ipaddr;
    return ;
}

const QString RDCHostInfo::getHostName(void) const
{
    return this->m_pHostName;
}

void RDCHostInfo::setHostName(QString hostName)
{
    this->m_pHostName = hostName;
    return ;
}

const sockaddr_in RDCHostInfo::toNative(void) const
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(struct sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_len = sizeof(struct sockaddr_in);
    addr.sin_port = htons(this->m_usPort);
    addr.sin_addr.s_addr = inet_addr(this->m_pIPAddress.toLatin1().constData());

    return addr;
}
