#ifndef RDCMESSAGEPOOL_H
#define RDCMESSAGEPOOL_H

#include <vector>
#include <mutex>
#include <memory>
#include <RDCMessage.h>

typedef std::shared_ptr<RDCMessage> MESSAGE_PTR;

class RDCMessage;
class RDCMessagePool
{
public:
    ~RDCMessagePool();
    static RDCMessagePool* pool(void);

    MESSAGE_PTR newMessage(void);
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
