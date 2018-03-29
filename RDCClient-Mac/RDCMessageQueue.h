#ifndef RDCMESSAGEQUEUE_H
#define RDCMESSAGEQUEUE_H

#include <vector>
#include <QMutex>

#define LOCK_BEGIN(m) m.lock();
#define LOCK_END(m) m.unlock()
#define LOCK(code) LOCK_BEGIN(m_pMutex) \
    do { \
        code \
    }while(0); \
    LOCK_END(this->m_pMutex)


template<typename T>
class RDCMessageQueue
{
public:
    RDCMessageQueue() {}

    bool empty(void)
    {
        bool isEmpty = true;
        LOCK({
                 isEmpty = this->m_MessageVector.empty();
             });
        return isEmpty;
    }

    int size(void)
    {
        int len = 0;
        LOCK({
                 len = this->m_MessageVector.size();
             });
        return len;
    }

    void push_back(T obj)
    {
        LOCK({
                 if(obj != nullptr)
                 {
                     this->m_MessageVector.push_back(obj);
                 }
             });
        return ;
    }

    T pop_front(void)
    {
        T obj = nullptr;
        LOCK({
                 if(this->m_MessageVector.empty() == false)
                 {
                     obj = this->m_MessageVector.front();
                     this->m_MessageVector.erase(this->m_MessageVector.begin());
                 }
             });
        return obj;
    }

    void clear(void)
    {
        LOCK({
                 while(this->m_MessageVector.empty() == false)
                 {
                     this->m_MessageVector.pop_back();
                 }
             });
        return ;
    }

private:
    std::vector<T> m_MessageVector;
    QMutex m_pMutex;
};

#endif // RDCMESSAGEQUEUE_H
