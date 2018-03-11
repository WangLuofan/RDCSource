#ifndef RDCMESSAGEPOOL_H
#define RDCMESSAGEPOOL_H

#include <vector>
#include <mutex>
#include <memory>

class RDCMessage;
class RDCMessagePool
{
public:
    ~RDCMessagePool();
    static RDCMessagePool* pool(void);

    std::shared_ptr<RDCMessage> newMessage(void);
    void retrieveMessage(RDCMessage*);

    void clear(void);

private:
    RDCMessagePool();

private:
    static RDCMessagePool* m_pObject;
    std::vector<RDCMessage*> m_pMessagePool;
    std::mutex m_pMutex;
};

#endif // RDCMESSAGEPOOL_H
