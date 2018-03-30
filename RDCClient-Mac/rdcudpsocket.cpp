#include "RDCUdpSocket.h"
#include "RDCHostInfo.h"
#include "RDCMessage.h"
#include "RDCUdpSocketEventHandler.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <QDebug>

#define kMaxBufferSize 5125
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

bool RDCUdpSocket::bindAtPort(unsigned short port)
{
    bool bSuccess = true;
    if(this->m_iFileDescriptor != -1)
    {
        struct sockaddr_in addr;
        bzero(&addr, sizeof(struct sockaddr_in));

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_len = sizeof(struct sockaddr_in);
        addr.sin_addr.s_addr = INADDR_ANY;

        int res = bind(this->m_iFileDescriptor,
                       (const struct sockaddr*)&addr, sizeof(struct sockaddr));
        bSuccess = (res == 0);
    }
    return bSuccess;
}

bool RDCUdpSocket::connectToHost(const RDCHostInfo* hostInfo)
{
    struct sockaddr_in addr = hostInfo->toNative();

    if(connect(this->m_iFileDescriptor, (const sockaddr*)&addr, sizeof(struct sockaddr)) == 0)
        return true;
    return false;
}

bool RDCUdpSocket::connectToHost(const char* ipAddr, unsigned short port)
{
    RDCHostInfo hostInfo(QString(ipAddr), port);
    return this->connectToHost(&hostInfo);
}

void RDCUdpSocket::sendMessage(RDCMessage* msg)
{
    ssize_t sndLen = write(this->m_iFileDescriptor, msg->data(), msg->size());
    if(sndLen == -1)
        qDebug() << strerror(errno);
    return ;
}

void RDCUdpSocket::receiveMessage(void)
{
    unsigned char buff[kMaxBufferSize] = {0};
    ssize_t recvLen = read(this->m_iFileDescriptor, buff, kMaxBufferSize);

    if(recvLen > 0)
    {
        RDCMessage* msg = RDCMessage::newMessage();
        msg->appendData(buff, recvLen);

        if(this->m_pSocketEventHandler != nullptr)
            this->m_pSocketEventHandler->onScreenDataReceived(this, msg);
    }
    return ;
}

RDCUdpSocketEventHandler *RDCUdpSocket::getSocketEventHandler(void) const
{
    return this->m_pSocketEventHandler;
}

void RDCUdpSocket::setSocketEventHandler(RDCUdpSocketEventHandler *pSocketEventHandler)
{
    this->m_pSocketEventHandler = pSocketEventHandler;
}
