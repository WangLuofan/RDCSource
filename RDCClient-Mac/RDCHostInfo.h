#ifndef RDCHOSTINFO_H
#define RDCHOSTINFO_H

#include <QString>
#include <netinet/in.h>

class RDCHostInfo
{
public:
    RDCHostInfo();
    RDCHostInfo(struct sockaddr*);
    RDCHostInfo(const QString, const unsigned short, const QString = nullptr);
    RDCHostInfo(const RDCHostInfo&);

    unsigned short getPort(void) const;
    void setPort(unsigned short);

    const QString getIPAddress(void) const;
    void setIPAddress(QString);

    const QString getHostName(void) const;
    void setHostName(QString);

    const struct sockaddr_in toNative(void) const;
private:
    QString m_pIPAddress;
    unsigned short m_usPort;
    QString m_pHostName;
};

#endif // RDCHOSTINFO_H
