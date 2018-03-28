#ifndef RDCUDPSOCKET_H
#define RDCUDPSOCKET_H

class RDCHostInfo;
class RDCMessage;
class RDCUdpSocketEventHandler;
class RDCUdpSocket
{
public:
    RDCUdpSocket();
    RDCUdpSocket(int);
    ~RDCUdpSocket();

    void close(void);
    bool bindAtPort(unsigned short);
    bool connectToHost(const RDCHostInfo*);
    bool connectToHost(const char*, unsigned short);
    void sendMessage(RDCMessage*);
    void receiveMessage(void);
    RDCUdpSocketEventHandler *getSocketEventHandler(void) const;
    void setSocketEventHandler(RDCUdpSocketEventHandler *pSocketEventHandler);

private:
    int m_iFileDescriptor;
    RDCUdpSocketEventHandler* m_pSocketEventHandler;
};

#endif // RDCUDPSOCKET_H
