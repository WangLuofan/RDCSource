#ifndef RDCMESSAGEQUEUE_H
#define RDCMESSAGEQUEUE_H

#include <vector>
#include <QMutex>
#include <RDCMessagePool.h>

class RDCMessageQueue
{
public:
    RDCMessageQueue();

    bool empty(void);
    int size(void);
    void push_back(MESSAGE_PTR);
    MESSAGE_PTR pop_front(void);
    void clear(void);

private:
    std::vector<MESSAGE_PTR> m_MessageVector;
    QMutex m_pMutex;
};

#endif // RDCMESSAGEQUEUE_H
