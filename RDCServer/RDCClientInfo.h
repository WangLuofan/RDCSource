#ifndef RDCCLIENTINFO_H
#define RDCCLIENTINFO_H

#include <QString>
#include <QMutex>

class RDCHostInfo;
class RDCTcpSocket;

class RDCClientInfo
{
public:
    typedef enum {
        ClientStatusOnline,
        ClientStatusOffline
    }ClientStatus;

    RDCClientInfo();
    ~RDCClientInfo();

    RDCClientInfo *getPeerInfo(void);
    void setPeerInfo(RDCClientInfo*);

    RDCTcpSocket *getSocket(void);
    void setSocket(RDCTcpSocket*);

    RDCHostInfo *getHostInfo(void);
    void setHostInfo(const QString, const unsigned short, const QString = nullptr);

    const QString getToken(void) const;
    void setToken(const QString);

    const QString getPassword(void) const;
    void setPassword(const QString);

    const QString getSystemVersion(void) const;
    void setSystemVersion(const QString);

    const QString getOnlineTimeStamp(void) const;
    void setOnlineTimeStamp(const QString);

    const QString getCurrentStatusDesc(void) const;
    void setCurrentStatus(ClientStatus);

    void setCurrentRowIndex(const int);
    int getCurrentRowIndex(void) const;

private:
    QString statusDescription(ClientStatus);

private:
    int m_iCurrentRowIndex;
    QMutex m_pMutex;

    RDCClientInfo* m_pPeerInfo;
    RDCTcpSocket* m_pSocket;
    RDCHostInfo* m_pHostInfo;
    QString m_pToken;
    QString m_pPassword;
    QString m_pSystemVersion;
    QString m_pOnlineTimeStamp;
    QString m_pCurrentStatusDesc;
    ClientStatus m_pCurrentStatus;
};

#endif // RDCCLIENTINFO_H
