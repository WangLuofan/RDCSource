#ifndef RDCSCREENDATASENDTHREAD_H
#define RDCSCREENDATASENDTHREAD_H

#include <QThread>

class QSemaphore;
class RDCMessageQueue;
class RDCUdpSocket;
class RDCScreenDataSendThread : public QThread
{
    Q_OBJECT
public:
    explicit RDCScreenDataSendThread(QObject* = nullptr, QSemaphore* = nullptr,
                                     RDCMessageQueue* = nullptr, RDCUdpSocket* = nullptr);

private:
    void run(void);

private:
    QSemaphore* m_pSemaphore;
    bool m_bShouldThreadExit;
    RDCMessageQueue* m_pMsgQueue;
    RDCUdpSocket* m_pSocket;
};

#endif // RDCSCREENDATASENDTHREAD_H
