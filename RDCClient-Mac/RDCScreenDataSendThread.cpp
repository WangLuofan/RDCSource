#include "RDCUdpSocket.h"
#include "RDCMessage.h"
#include "RDCMessagePool.h"
#include "RDCMessageQueue.h"
#include "RDCScreenDataSendThread.h"

#include <QSemaphore>

RDCScreenDataSendThread::RDCScreenDataSendThread(QObject* parent, QSemaphore* sem,
                                                 RDCMessageQueue* msgQueue, RDCUdpSocket* socket) :
    QThread(parent), m_pSemaphore(sem), m_bShouldThreadExit(false), m_pMsgQueue(msgQueue), m_pSocket(socket)
{
}

void RDCScreenDataSendThread::run()
{
    while(!m_bShouldThreadExit)
    {
        this->m_pSemaphore->acquire();

        if(this->m_pMsgQueue->empty() == false)
        {
            MESSAGE_PTR msg = this->m_pMsgQueue->pop_front();
            if(msg != nullptr)
            {
                this->m_pSocket->sendMessage(msg.get());
            }
        }
        QThread::msleep(10);
    }
    return ;
}
