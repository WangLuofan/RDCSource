#include "RDCMessageQueue.h"

#define LOCK_BEGIN(m) m.lock();
#define LOCK_END(m) m.unlock()
#define LOCK(code) LOCK_BEGIN(m_pMutex) \
    do { \
        code \
    }while(0); \
    LOCK_END(this->m_pMutex)

RDCMessageQueue::RDCMessageQueue()
{    
}

bool RDCMessageQueue::empty(void)
{
    bool isEmpty = true;
    LOCK({
             isEmpty = this->m_MessageVector.empty();
         });
    return isEmpty;
}

int RDCMessageQueue::size(void)
{
    int len = 0;
    LOCK({
             len = this->m_MessageVector.size();
         });
    return len;
}

void RDCMessageQueue::push_back(MESSAGE_PTR msg)
{
    LOCK({
             if(msg != nullptr)
             {
                 this->m_MessageVector.push_back(msg);
             }
         });
    return ;
}

MESSAGE_PTR RDCMessageQueue::pop_front(void)
{
    MESSAGE_PTR msg = nullptr;
    LOCK({
             if(this->m_MessageVector.empty() == false)
             {
                 msg = this->m_MessageVector.front();
                 this->m_MessageVector.erase(this->m_MessageVector.begin());
             }
         });
    return msg;
}

void RDCMessageQueue::clear()
{
    LOCK({
             while(this->m_MessageVector.empty() == false)
             {
                 this->m_MessageVector.pop_back();
             }
         });
    return ;
}
