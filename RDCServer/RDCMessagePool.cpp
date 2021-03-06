#include "RDCMessage.h"
#include "RDCMessagePool.h"

#include <QDebug>

#define LOCK_BEGIN(m) m.lock();
#define LOCK_END(m) m.unlock()
#define LOCK(code) LOCK_BEGIN(m_pMutex) \
    do { \
        code \
    }while(0); \
    LOCK_END(this->m_pMutex)

RDCMessagePool* RDCMessagePool::m_pObject = nullptr;
RDCMessagePool *RDCMessagePool::pool()
{
    if(RDCMessagePool::m_pObject == nullptr)
        RDCMessagePool::m_pObject = new RDCMessagePool();
    return RDCMessagePool::m_pObject;
}

void retrieveMessage(RDCMessage* msg)
{
    RDCMessagePool::pool()->retrieveMessage(msg);
    return ;
}

MESSAGE_PTR RDCMessagePool::newMessage(void)
{
    std::shared_ptr<RDCMessage> msg_ptr = nullptr;
    LOCK({
             if(this->m_pMessagePool.size() > 0)
             {
                 RDCMessage* message = this->m_pMessagePool.back();
                 this->m_pMessagePool.pop_back();

                 msg_ptr = MESSAGE_PTR(message, ::retrieveMessage);
             }
             else
             {
                 msg_ptr = MESSAGE_PTR(RDCMessage::newMessage(), ::retrieveMessage);
             }
         });
    return msg_ptr;
}

void RDCMessagePool::retrieveMessage(RDCMessage* msg)
{
    LOCK({
             msg->realloc();
             this->m_pMessagePool.push_back(msg);
         });
    return ;
}

void RDCMessagePool::clear(void)
{
    LOCK({
             while(!this->m_pMessagePool.empty())
             {
                 RDCMessage* message = this->m_pMessagePool.at(this->m_pMessagePool.size() - 1);
                 this->m_pMessagePool.pop_back();

                 delete message;
             }
         });
    return ;
}

RDCMessagePool::RDCMessagePool()
{
}

RDCMessagePool::~RDCMessagePool()
{
    this->clear();
}
