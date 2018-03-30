#ifndef RDCSCREENDATASENDTHREAD_H
#define RDCSCREENDATASENDTHREAD_H

#include "RDCMessageQueue.h"

#include <QThread>

class QSemaphore;
class RDCUdpSocket;
class RDCMessage;
class RDCScreenDataSendThread : public QThread
{
    Q_OBJECT
public:
    explicit RDCScreenDataSendThread(QObject* = nullptr, QSemaphore* = nullptr,
                                     RDCMessageQueue<RDCMessage*>* = nullptr, RDCUdpSocket* = nullptr);

private:
    void run(void);

private:
    QSemaphore* m_pSemaphore;
    bool m_bShouldThreadExit;
    RDCMessageQueue<RDCMessage*>* m_pMsgQueue;
    RDCUdpSocket* m_pSocket;
};

#endif // RDCSCREENDATASENDTHREAD_H
