#ifndef RDCTCPSOCKET_H
#define RDCTCPSOCKET_H

struct event_base;
struct evconnlistener;
struct bufferevent;

class RDCMessage;
class RDCTcpSocketEventHandler;

class RDCTcpSocket
{
public:
    RDCTcpSocket();
    RDCTcpSocket(int);
    ~RDCTcpSocket();

    void Shutdown(void);
    void ExitEventBase(void);
    void ListenAtPort(unsigned short);

    int getFileDescriptor() const;
    void setFileDescriptor(int);

    struct event_base* getEventBase(void) const;
    void setEventBase(struct event_base*);

    RDCTcpSocketEventHandler *getEventHandler() const;
    void setEventHandler(RDCTcpSocketEventHandler *);

    struct bufferevent* getBufferEvent(void) const;
    void setBufferEvent(struct bufferevent *);

    void sendMessage(RDCMessage *);
private:
    int m_FileDescriptor;
    struct evconnlistener* m_pConnListener;
    struct event_base* m_pEventBase;
    struct bufferevent* m_pBufferEvent;

    RDCTcpSocketEventHandler* m_pEventHandler;
};

#endif // RDCTCPSOCKET_H
