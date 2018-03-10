#ifndef RDCMESSAGE_H
#define RDCMESSAGE_H

#include "RDCServiceCommand.h"

#include <QString>

class RDCMessage
{
private:
    RDCMessage();
    RDCMessage(size_t);

public:
    ~RDCMessage();

    const size_t size(void) const;
    void reset(void);

    const unsigned char* data(void) const;
    unsigned char* data(void);
    int current(void);

    void realloc(void);

    ServiceCommand serviceCommand(void);

    unsigned char nextChar(void);
    unsigned short nextShort(void);
    unsigned int nextInteger(void);
    unsigned char* nextData(int);
    QString nextString(int);

    void appendChar(unsigned char);
    void appendShort(unsigned short);
    void appendInteger(unsigned int);
    void appendData(const unsigned char*, int);
    void appendString(QString&);

    void mergeMessage(const RDCMessage*);

    void replaceChar(unsigned char, int);
    void replaceShort(unsigned short, int);
    void replaceInteger(unsigned int, int);
    void replaceData(const unsigned char*, int, int);
    void replaceString(QString&, int);

    void shrinkToFit(int, bool);

    static RDCMessage* newMessage(void);
    static RDCMessage* newMessage(size_t);
    static RDCMessage* newMessage(ServiceCommand);
    static RDCMessage* newMessage(const unsigned char *, size_t);
    static RDCMessage* newMessage(QString&);

private:
    void strechToFit(const size_t);

private:
    unsigned char* m_pPartialData;
    unsigned char* m_pData;
    int m_iLength;
    int m_pCurrent;
    int m_iTotalLength;
};

#endif // RDCMESSAGE_H
