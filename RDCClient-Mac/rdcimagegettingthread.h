#ifndef RDCIMAGEGETTINGTHREAD_H
#define RDCIMAGEGETTINGTHREAD_H

#include "RDCMessageQueue.h"

#include <QThread>

class QImage;
class QSemaphore;
class RDCImageGettingThread : public QThread
{
    Q_OBJECT
public:
    explicit RDCImageGettingThread(QObject* = nullptr,
                                   QSemaphore* = nullptr, RDCMessageQueue<QImage*>* = nullptr);

private:
    void run(void);

private:
    QSemaphore* m_pSemaphore;
    RDCMessageQueue<QImage*>* m_pImageQueue;
};

#endif // RDCIMAGEGETTINGTHREAD_H
