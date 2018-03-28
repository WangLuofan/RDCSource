#ifndef RDCSCREENMSGRECEIVEDTHREAD_H
#define RDCSCREENMSGRECEIVEDTHREAD_H

#include <QThread>

class RDCUdpSocket;
class RDCScreenMsgReceivedThread : public QThread
{
    Q_OBJECT
public:
    explicit RDCScreenMsgReceivedThread(QObject* = nullptr, RDCUdpSocket* = nullptr);

    void run(void);
    void cancel(void);

private:
    RDCUdpSocket* m_pScreenSocket;
    bool m_bShouldThreadExit;
};

#endif // RDCSCREENMSGRECEIVEDTHREAD_H
