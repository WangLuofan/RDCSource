#ifndef RDCSCREENDATASENDTHREAD_H
#define RDCSCREENDATASENDTHREAD_H

#include "RDCMessagePool.h"
#include "RDCMessageQueue.h"

#include <QThread>

class QSemaphore;
class RDCUdpSocket;
class RDCScreenDataSendThread : public QThread
{
    Q_OBJECT
public:
    explicit RDCScreenDataSendThread(QObject* = nullptr, QSemaphore* = nullptr,
                                     RDCMessageQueue<MESSAGE_PTR>* = nullptr, RDCUdpSocket* = nullptr);

private:
    void run(void);

private:
    QSemaphore* m_pSemaphore;
    bool m_bShouldThreadExit;
    RDCMessageQueue<MESSAGE_PTR>* m_pMsgQueue;
    RDCUdpSocket* m_pSocket;
};

#endif // RDCSCREENDATASENDTHREAD_H
