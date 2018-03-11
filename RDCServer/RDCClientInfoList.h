#ifndef RDCCLIENTINFOLIST_H
#define RDCCLIENTINFOLIST_H

#include <vector>
#include <mutex>

class RDCTcpSocket;
class RDCClientInfo;

class RDCClientInfoList
{
public:
    static RDCClientInfoList* sharedInstance(void);

    void addClientInfo(RDCClientInfo*);
    RDCClientInfo* getClientInfo(const RDCTcpSocket*);
    RDCClientInfo* getClientInfo(const int);
    RDCClientInfo* getClientInfo(const bool);

    void removeClientInfo(const RDCClientInfo*);
    void removeClientInfo(const int);
    void clear(void);

    bool empty(void);
    size_t size(void);
private:
    RDCClientInfoList();

private:
    static RDCClientInfoList* m_StaticObject;
    std::vector<RDCClientInfo*> m_pClientInfoList;
    std::mutex m_pMutex;
};

#endif // RDCCLIENTINFOLIST_H
