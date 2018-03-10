#include <string.h>
#include "RDCHostInfo.h"

#include <netinet/in.h>
#include <arpa/inet.h>

RDCHostInfo::RDCHostInfo() :
    m_usPort(0)
{
    memset(this->m_pIPAddress, 0, MAX_IPADDR_LENGTH * sizeof(char));
    memset(this->m_pHostName, 0, MAX_HOSTNAME_LENGTH * sizeof(char));
}

RDCHostInfo::RDCHostInfo(struct sockaddr* addr)
{
    this->m_usPort = ntohs(((struct sockaddr_in*)addr)->sin_port);

    const char* ipaddr = inet_ntoa(((struct sockaddr_in*)addr)->sin_addr);
    memcpy(this->m_pIPAddress, ipaddr, strlen(ipaddr));
    memset(this->m_pHostName, 0, MAX_HOSTNAME_LENGTH * sizeof(char));
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

const char* RDCHostInfo::getIPAddress(void) const
{
    return this->m_pIPAddress;
}

void RDCHostInfo::setIPAddress(const char* ipaddr)
{
    memcpy(this->m_pIPAddress, ipaddr, strlen(ipaddr));
    return ;
}

const char* RDCHostInfo::getHostName(void) const
{
    return this->m_pHostName;
}

void RDCHostInfo::setHostName(const char* hostName)
{
    memcpy(this->m_pHostName, hostName, strlen(hostName));
    return ;
}
