#ifndef RDCHOSTINFO_H
#define RDCHOSTINFO_H

struct sockaddr;
class RDCHostInfo
{
public:
    RDCHostInfo();
    RDCHostInfo(struct sockaddr*);
    RDCHostInfo(const RDCHostInfo&);

    unsigned short getPort(void) const;
    void setPort(unsigned short);

    const char* getIPAddress(void) const;
    void setIPAddress(const char*);

    const char* getHostName(void) const;
    void setHostName(const char*);
private:
#define MAX_IPADDR_LENGTH 20
#define MAX_HOSTNAME_LENGTH 256

    char m_pIPAddress[MAX_IPADDR_LENGTH];
    unsigned short m_usPort;
    char m_pHostName[MAX_HOSTNAME_LENGTH];
};

#endif // RDCHOSTINFO_H
