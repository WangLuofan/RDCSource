#include "RDCHostInfo.h"
#include "RDCMessage.h"
#include "RDCTcpSocket.h"
#include "RDCMessagePool.h"
#include "RDCTcpSocketEventHandler.h"

#include <strings.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

RDCTcpSocket::RDCTcpSocket() : m_FileDescriptor(-1), m_pBufferEvent(nullptr),
    m_pConnListener(nullptr), m_pEventBase(nullptr)
{
}

RDCTcpSocket::RDCTcpSocket(int fd) : m_FileDescriptor(fd), m_pBufferEvent(nullptr),
    m_pConnListener(nullptr), m_pEventBase(nullptr)
{
}

RDCTcpSocket::~RDCTcpSocket()
{
    this->Shutdown();

    if(this->m_pConnListener != nullptr)
    {
        evconnlistener_free(this->m_pConnListener);
        this->m_pConnListener = nullptr;
    }

    if(this->m_pEventHandler != nullptr)
        this->m_pEventHandler = nullptr;

    if(this->m_pBufferEvent != nullptr)
    {
        bufferevent_free(this->m_pBufferEvent);
        this->m_pBufferEvent = nullptr;
    }
}

void RDCTcpSocket::Shutdown(void)
{
    if(this->m_FileDescriptor != -1)
        evutil_closesocket(this->m_FileDescriptor);

    return ;
}

void RDCTcpSocket::ExitEventBase(void)
{
    if(this->m_pEventBase != nullptr)
    {
        event_base_loopexit(this->m_pEventBase, nullptr);
        event_base_free(this->m_pEventBase);
        this->m_pEventBase = nullptr;
    }

    return ;
}

void RDCTcpSocket::ListenAtPort(unsigned short port)
{
    evthread_use_pthreads();
    this->m_pEventBase = event_base_new();

    struct sockaddr_in addr;
    bzero(&addr, sizeof(struct sockaddr));

    addr.sin_family = AF_INET;
    addr.sin_len = sizeof(struct sockaddr_in);
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    void accept_call_back(struct evconnlistener*,
                          evutil_socket_t, struct sockaddr*, int, void*);
    this->m_pConnListener = evconnlistener_new_bind(this->m_pEventBase, accept_call_back, (void*)this,
                                                    LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 5,
                                                    (const sockaddr*)&addr, sizeof(struct sockaddr));


    event_base_dispatch(this->m_pEventBase);
    return ;
}

int RDCTcpSocket::getFileDescriptor() const
{
    return this->m_FileDescriptor;
}

void RDCTcpSocket::setFileDescriptor(int FileDescriptor)
{
    this->m_FileDescriptor = FileDescriptor;
}

struct event_base *RDCTcpSocket::getEventBase() const
{
    return this->m_pEventBase;
}

void RDCTcpSocket::setEventBase(struct event_base *evb)
{
    this->m_pEventBase = evb;
    return ;
}

RDCTcpSocketEventHandler *RDCTcpSocket::getEventHandler(void) const
{
    return this->m_pEventHandler;
}

void RDCTcpSocket::setEventHandler(RDCTcpSocketEventHandler *pEventHandler)
{
    this->m_pEventHandler = pEventHandler;
}

bufferevent *RDCTcpSocket::getBufferEvent() const
{
    return this->m_pBufferEvent;
}

void RDCTcpSocket::setBufferEvent(struct bufferevent* evt)
{
    this->m_pBufferEvent = evt;
    return ;
}

void RDCTcpSocket::sendMessage(RDCMessage* msg)
{
    struct evbuffer* output = bufferevent_get_output(this->getBufferEvent());
    evbuffer_add(output, msg->data(), msg->size());
    return ;
}

void accept_call_back(struct evconnlistener* listener,
                      evutil_socket_t socket, struct sockaddr* addr, int socklen, void* ctx)
{
    RDCTcpSocket* listenSocket = (RDCTcpSocket*)ctx;

    RDCTcpSocket* newSocket = new RDCTcpSocket(socket);
    newSocket->setEventHandler(listenSocket->getEventHandler());

    RDCHostInfo hostInfo(addr);

    struct bufferevent* bvt = bufferevent_socket_new((struct event_base*)listenSocket->getEventBase(), socket,
                                                     BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    bufferevent_enable(bvt, EV_READ | EV_WRITE | EV_PERSIST);

    void read_call_back(struct bufferevent*, void*);
    void event_dispatched_call_back(struct bufferevent*, short, void*);
    bufferevent_setcb(bvt, read_call_back, nullptr, event_dispatched_call_back, (void*)newSocket);

    if(listenSocket->getEventHandler() != nullptr)
        listenSocket->getEventHandler()->onClientConnected(newSocket, &hostInfo);
    return ;
}

void read_call_back(struct bufferevent* bev, void* ctx)
{
    RDCTcpSocket* socket = (RDCTcpSocket*)ctx;

    struct evbuffer* input = bufferevent_get_input(bev);
    size_t buflen = evbuffer_get_length(input);

    unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * buflen);
    memset(buffer, 0, sizeof(unsigned char) * buflen);

    evbuffer_remove(input, buffer, buflen);

    std::shared_ptr<RDCMessage> msg = RDCMessagePool::pool()->newMessage();
    msg.get()->appendData(buffer, buflen);

    free(buffer);

    if(socket->getEventHandler() != nullptr)
        socket->getEventHandler()->onMessageReceived(socket, msg.get());

    return ;
}

void event_dispatched_call_back(struct bufferevent *bev, short what, void *ctx)
{
    RDCTcpSocket* socket = (RDCTcpSocket*)ctx;
    if(what & BEV_EVENT_CONNECTED)
    {
        if(socket->getEventHandler() != nullptr)
            socket->getEventHandler()->onConnectedToServer(socket);
        return ;
    }
    else if(what & BEV_EVENT_EOF)
    {
        if(socket->getEventHandler() != nullptr)
            socket->getEventHandler()->onClientDisconnected(socket);
    }
    else if(what & BEV_EVENT_TIMEOUT)
    {
        if(socket->getEventHandler() != nullptr)
            socket->getEventHandler()->onTimeOutEventOccured(socket);
    }
    else
    {
        if(socket->getEventHandler() != nullptr)
            socket->getEventHandler()->onErrorOccured(socket, strerror(errno));
    }

    bufferevent_free(bev);
    socket->setBufferEvent(nullptr);
    return ;
}
