#include "RDCUdpSocket.h"
#include "RDCMessage.h"
#include "RDCScreenDataSendThread.h"

#include <QSemaphore>

RDCScreenDataSendThread::RDCScreenDataSendThread(QObject* parent, QSemaphore* sem,
                                                 RDCMessageQueue<MESSAGE_PTR>* msgQueue,
                                                 RDCUdpSocket* socket) :
    QThread(parent), m_pSemaphore(sem), m_bShouldThreadExit(false), m_pMsgQueue(msgQueue), m_pSocket(socket)
{
}

void RDCScreenDataSendThread::run(void)
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
        QThread::usleep(40);
    }
    return ;
}
