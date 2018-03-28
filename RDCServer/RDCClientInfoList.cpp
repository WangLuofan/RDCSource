#include "RDCClientInfo.h"
#include "RDCClientInfoList.h"

#include <QString>

#define LOCK_BEGIN(m) m.lock();
#define LOCK_END(m) m.unlock()
#define LOCK(code) LOCK_BEGIN(m_pMutex) \
    do { \
        code \
    }while(0); \
    LOCK_END(this->m_pMutex)

RDCClientInfoList* RDCClientInfoList::m_StaticObject = nullptr;
RDCClientInfoList *RDCClientInfoList::sharedInstance()
{
    if(RDCClientInfoList::m_StaticObject == nullptr)
        RDCClientInfoList::m_StaticObject = new RDCClientInfoList();
    return RDCClientInfoList::m_StaticObject;
}

void RDCClientInfoList::addClientInfo(RDCClientInfo* clientInfo)
{
    LOCK({
             this->m_pClientInfoList.push_back(clientInfo);
         });
    return ;
}

RDCClientInfo *RDCClientInfoList::getClientInfo(const RDCTcpSocket* socket)
{
    RDCClientInfo* clientInfo = nullptr;
    LOCK({
             for(auto it : this->m_pClientInfoList)
             {
                 if(it->getSocket() == socket)
                 {
                     clientInfo = it;
                     break ;
                 }
             }
         });
    return clientInfo;
}

RDCClientInfo *RDCClientInfoList::getClientInfo(const int idx)
{
    RDCClientInfo* clientInfo = nullptr;
    LOCK({
             if(idx > this->m_pClientInfoList.size())
                break ;

             clientInfo = this->m_pClientInfoList.at(idx);
         });
    return clientInfo;
}

RDCClientInfo *RDCClientInfoList::getClientInfo(const char* token)
{
    RDCClientInfo* clientInfo = nullptr;
    LOCK({
             QString tokenStr = QString(token);
             if(tokenStr.length() > 0)
             {
                 for(auto info : this->m_pClientInfoList)
                 {
                     if(info->getToken().compare(tokenStr) == 0)
                     {
                         clientInfo = info;
                         break;
                     }
                 }
             }
         });
    return clientInfo;
}

RDCClientInfo *RDCClientInfoList::getClientInfo(const bool bFirst)
{
    RDCClientInfo* clientInfo = nullptr;
    LOCK({
             if(bFirst)
             {
                 if(this->m_pClientInfoList.size() > 0)
                    clientInfo = this->m_pClientInfoList.at(0);
             }
             else
             {
                 if(this->m_pClientInfoList.size() > 0)
                    clientInfo = this->m_pClientInfoList.at(this->m_pClientInfoList.size() - 1);
             }
         });
    return clientInfo;
}

void RDCClientInfoList::removeClientInfo(const RDCClientInfo* clientInfo)
{
    LOCK({
             if(clientInfo != nullptr)
             {
                 for(auto it = this->m_pClientInfoList.begin(); it != this->m_pClientInfoList.end(); ++it)
                 {
                     if(*it == clientInfo)
                     {
                         this->m_pClientInfoList.erase(it);
                         break;
                     }
                 }
                 delete clientInfo;
             }

         });
    return ;
}

void RDCClientInfoList::removeClientInfo(const int idx)
{
    LOCK({
             if(idx > this->m_pClientInfoList.size())
                break ;
             auto it = this->m_pClientInfoList.begin() + idx;
             this->m_pClientInfoList.erase(it);

             delete *it;
         });
    return ;
}

void RDCClientInfoList::clear()
{
    LOCK({
             while(!this->m_pClientInfoList.empty())
             {
                 RDCClientInfo* cliInfo = this->m_pClientInfoList.at(this->m_pClientInfoList.size() - 1);
                 this->m_pClientInfoList.pop_back();

                 delete cliInfo;
             }
         });
    return ;
}

bool RDCClientInfoList::empty()
{
    bool bEmpty = false;
    LOCK({
             bEmpty = this->m_pClientInfoList.empty();
         });
    return bEmpty;
}

size_t RDCClientInfoList::size()
{
    size_t len = 0;
    LOCK({
             len = this->m_pClientInfoList.size();
         });
    return len;
}

RDCClientInfoList::RDCClientInfoList()
{
}

