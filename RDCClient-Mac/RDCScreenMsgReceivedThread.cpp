#include "RDCUdpSocket.h"
#include "RDCMessage.h"
#include "RDCUdpSocketEventHandler.h"
#include "RDCScreenMsgReceivedThread.h"

RDCScreenMsgReceivedThread::RDCScreenMsgReceivedThread(QObject* parent, RDCUdpSocket* socket) :
    QThread(parent), m_pScreenSocket(socket), m_bShouldThreadExit(false)
{

}

void RDCScreenMsgReceivedThread::run()
{
    while(!this->m_bShouldThreadExit)
    {
        this->m_pScreenSocket->receiveMessage();
        QThread::msleep(1);
    }
    return ;
}

void RDCScreenMsgReceivedThread::cancel()
{
    this->m_bShouldThreadExit = true;
    return ;
}
