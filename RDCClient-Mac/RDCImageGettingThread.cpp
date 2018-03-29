#include "RDCImageGettingThread.h"

RDCImageGettingThread::RDCImageGettingThread(QObject* parent, QSemaphore* sem,
                                             RDCMessageQueue<QImage*>* queue) :
    QThread(parent), m_pSemaphore(sem), m_pImageQueue(queue)
{
}

void RDCImageGettingThread::run(void)
{
    return ;
}
