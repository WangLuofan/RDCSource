#include "RDCUdpSocket.h"
#include "RDCHostInfo.h"
#include "RDCMessage.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

RDCUdpSocket::RDCUdpSocket()
{
    this->m_iFileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

RDCUdpSocket::RDCUdpSocket(int fd) : m_iFileDescriptor(fd)
{
}

RDCUdpSocket::~RDCUdpSocket()
{
    this->close();
}

void RDCUdpSocket::close(void)
{
    if(this->m_iFileDescriptor != -1)
    {
        shutdown(this->m_iFileDescriptor, SHUT_RDWR);
        this->m_iFileDescriptor = -1;
    }
    return ;
}

void RDCUdpSocket::connectToHost(const RDCHostInfo* hostInfo)
{
    struct sockaddr_in addr = hostInfo->toNative();

    if(connect(this->m_iFileDescriptor, (const sockaddr*)&addr, sizeof(struct sockaddr)) == 0)
    {

    }
    return ;
}

void RDCUdpSocket::connectToHost(const char* ipAddr, unsigned short port)
{
    RDCHostInfo hostInfo(QString(ipAddr), port);
    return this->connectToHost(&hostInfo);
}

void RDCUdpSocket::sendMessage(RDCMessage* msg)
{
    send(this->m_iFileDescriptor, msg->data(), msg->size(), 0);
    return ;
}
