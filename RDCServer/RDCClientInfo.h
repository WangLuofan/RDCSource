#ifndef RDCCLIENTINFO_H
#define RDCCLIENTINFO_H

class RDCHostInfo;
class RDCTcpSocket;

class RDCClientInfo
{
public:
    RDCClientInfo();
    ~RDCClientInfo();

    RDCClientInfo *getPeerInfo(void) const;
    void setPeerInfo(RDCClientInfo*);

    RDCTcpSocket *getSocket(void) const;
    void setSocket(RDCTcpSocket*);

    RDCHostInfo *getHostInfo(void) const;
    void setHostInfo(const RDCHostInfo*);

    const char *getToken(void) const;
    void setToken(const char*);

    const char* getPassword(void) const;
    void setPassword(const char*);

    const char* getSystemVersion(void) const;
    void setSystemVersion(const char*);

    const char* getOnlineTimeStamp(void) const;
    void setOnlineTimeStamp(const char*);

    const char* getCurrentStatus(void) const;
    void setCurrentStatus(const char*);
private:
#define CLI_DATA_LEN 12
#define SYS_VERS_LEN 128

    RDCClientInfo* m_pPeerInfo;
    RDCTcpSocket* m_pSocket;
    RDCHostInfo* m_pHostInfo;
    char m_pToken[CLI_DATA_LEN];
    char m_pPassword[CLI_DATA_LEN];
    char m_pSystemVersion[SYS_VERS_LEN];
    char m_pOnlineTimeStamp[SYS_VERS_LEN];
    char m_pCurrentStatus[SYS_VERS_LEN];
};

#endif // RDCCLIENTINFO_H
