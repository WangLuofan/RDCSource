#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RDCMessage.h"

#define SAFE_DELETE(ptr) if(ptr != nullptr) { \
    free(ptr); \
    ptr = nullptr; \
}

const static int strech_size = 1024;

RDCMessage::RDCMessage() :
    m_iLength(0), m_pCurrent(0), m_iTotalLength(1024), m_pPartialData(nullptr)
{
    this->m_pData = (unsigned char*)malloc(sizeof(unsigned char) * m_iTotalLength);
    memset(this->m_pData, 0, sizeof(char) * m_iTotalLength);
}

RDCMessage::RDCMessage(size_t len) :
    m_iLength(0), m_pCurrent(0), m_iTotalLength(len), m_pPartialData(nullptr)
{
    this->m_pData = (unsigned char*)malloc(sizeof(unsigned char) * m_iTotalLength);
    memset(this->m_pData, 0, sizeof(char) * m_iTotalLength);
}

RDCMessage::~RDCMessage()
{
    SAFE_DELETE(this->m_pPartialData);
    SAFE_DELETE(this->m_pData);
}

RDCMessage* RDCMessage::newMessage(void)
{
    RDCMessage* msg = new RDCMessage();

    return msg;
}

RDCMessage* RDCMessage::newMessage(size_t size)
{
    RDCMessage* msg = new RDCMessage(size);
    return msg;
}

RDCMessage* RDCMessage::newMessage(ServiceCommand cmd)
{
    RDCMessage* msg = new RDCMessage();
    msg->appendChar((unsigned char)cmd);

    return msg;
}

RDCMessage* RDCMessage::newMessage(const unsigned char * data, size_t len)
{
    RDCMessage* msg = new RDCMessage();
    msg->appendData(data, len);

    return msg;
}

RDCMessage* RDCMessage::newMessage(QString & data)
{
    RDCMessage* msg = new RDCMessage();
    msg->appendData((const unsigned char*)data.toLatin1().data(), data.length());

    return msg;
}

void RDCMessage::replaceChar(unsigned char val, int position)
{
    if(this->m_iLength < position + sizeof(unsigned char))
        return ;
    memcpy(this->m_pData + position, &val, sizeof(unsigned char));
    return ;
}

void RDCMessage::replaceShort(unsigned short val, int position)
{
    if(this->m_iLength < position + sizeof(unsigned short))
        return ;
    memcpy(this->m_pData + position, &val, sizeof(unsigned short));
    return ;
}

void RDCMessage::replaceInteger(unsigned int val, int position)
{
    if(this->m_iLength < position + sizeof(unsigned int))
        return ;
    memcpy(this->m_pData + position, &val, sizeof(unsigned short));
    return ;
}

void RDCMessage::replaceData(const unsigned char * val, int position, int length)
{
    if(this->m_iLength < position + length)
        return ;
    memcpy(this->m_pData + position, val, sizeof(unsigned char) * length);
    return ;
}

void RDCMessage::replaceString(QString& str, int position)
{
    if(this->m_iLength < position + str.length())
        return ;
    this->replaceData((const unsigned char*)str.toLatin1().data(), position, str.length());
    return ;
}

void RDCMessage::strechToFit(const size_t length)
{
    if(this->m_iLength + length < this->m_iTotalLength)
        return ;

    this->m_pData = (unsigned char*)::realloc(this->m_pData, this->m_iLength + length + strech_size);
    if(this->m_pData != nullptr)
    {
        this->m_iTotalLength = this->m_iLength + length + strech_size;
        memset(this->m_pData + this->m_iLength, 0, sizeof(length + strech_size));
    }

    return ;
}

void RDCMessage::appendChar(unsigned char data)
{
    this->strechToFit(sizeof(unsigned char));

    memcpy(this->m_pData + this->m_iLength, &data, sizeof(unsigned char));
    this->m_iLength += sizeof(unsigned char);

    return ;
}

void RDCMessage::appendShort(unsigned short data)
{
    this->strechToFit(sizeof(unsigned short));

    memcpy(this->m_pData + this->m_iLength, &data, sizeof(unsigned short));
    this->m_iLength += sizeof(unsigned short);

    return ;
}

void RDCMessage::appendInteger(unsigned int data)
{
    this->strechToFit(sizeof(unsigned int));

    memcpy(this->m_pData + this->m_iLength, &data, sizeof(unsigned int));
    this->m_iLength += sizeof(unsigned int);

    return ;
}

void RDCMessage::appendData(const unsigned char *data, int len)
{
    this->strechToFit(len);

    memcpy(this->m_pData + this->m_iLength, data, sizeof(char) * len);
    this->m_iLength += sizeof(char) * len;

    return ;
}

void RDCMessage::appendString(QString& data)
{
    return this->appendData((const unsigned char*)data.toLatin1().data(), data.length());
}

void RDCMessage::mergeMessage(const RDCMessage * msg)
{
    this->strechToFit(msg->size());     //调整空间
    //添加数据
    this->appendData(msg->data(), msg->size());

    return ;
}

void RDCMessage::shrinkToFit(int offset, bool front)
{
    if(front)
    {
        //从头部开始缩小, 得到缩小后的内存
        int size = this->m_iLength - offset;

        //首先开辟一块新内存
        unsigned char* tmpBuf = (unsigned char*)malloc(sizeof(unsigned char) * size);
        memset(tmpBuf, 0, sizeof(unsigned char) * size);

        //将数据复制到新内存
        memcpy(tmpBuf, this->m_pData + offset, size);

        //调整指针
        if(this->m_pCurrent < offset)
            this->m_pCurrent = 0;
        else
            this->m_pCurrent -= offset;

        //释放旧内存
        SAFE_DELETE(this->m_pData);

        //调整大小
        this->m_iLength = size;
        this->m_iTotalLength = size;

        //指向新内存
        this->m_pData = tmpBuf;
    }
    else
    {
        //从尾部开始缩小
        this->m_pData = (unsigned char*)::realloc(this->m_pData, sizeof(unsigned char) * offset);
        this->m_iLength = offset;
        this->m_iTotalLength = offset;

        //如果current超出范围，缩小至最后一个字节
        if(this->m_pCurrent > offset)
            this->m_pCurrent = offset - 1;
    }
    return ;
}

void RDCMessage::realloc(void)
{
    SAFE_DELETE(this->m_pPartialData);
    SAFE_DELETE(this->m_pData);

    this->m_iLength = 0;
    this->m_pCurrent = 0;
    this->m_iTotalLength = 1024;

    this->m_pData = (unsigned char*)malloc(sizeof(unsigned char) * m_iTotalLength);
    memset(this->m_pData, 0, sizeof(char) * m_iTotalLength);

    return ;
}

void RDCMessage::reset(void)
{
    this->m_pCurrent = 0;
    SAFE_DELETE(this->m_pPartialData);

    return ;
}

int RDCMessage::current(void)
{
    return this->m_pCurrent;
}

const unsigned char* RDCMessage::data(void) const
{
    return this->m_pData;
}

unsigned char* RDCMessage::data(void)
{
    return this->m_pData;
}

const size_t RDCMessage::size(void) const
{
    return this->m_iLength;
}

ServiceCommand RDCMessage::serviceCommand(void)
{
    return (ServiceCommand)this->nextInteger();
}

unsigned char RDCMessage::nextChar(void)
{
    unsigned char data = *((unsigned char*)(this->m_pData + this->m_pCurrent));
    this->m_pCurrent += sizeof(unsigned char);

    return data;
}

unsigned short RDCMessage::nextShort(void)
{
    unsigned short data = *((unsigned short*)(this->m_pData + this->m_pCurrent));
    this->m_pCurrent += sizeof(unsigned short);

    return data;
}

unsigned int RDCMessage::nextInteger(void)
{
    unsigned int data = *((unsigned int*)(this->m_pData + this->m_pCurrent));
    this->m_pCurrent += sizeof(unsigned int);

    return data;
}

unsigned char* RDCMessage::nextData(int len)
{
    SAFE_DELETE(this->m_pPartialData);

    this->m_pPartialData = (unsigned char*)malloc(sizeof(unsigned char) * len);
    memcpy(this->m_pPartialData,
           this->m_pData + this->m_pCurrent, sizeof(unsigned char) * len);
    this->m_pCurrent += sizeof(unsigned char) * len;

    return this->m_pPartialData;
}

QString RDCMessage::nextString(int len)
{
    this->nextData(len);

    //为字符串补上一个结束字符\0
    this->m_pPartialData = (unsigned char*)::realloc(this->m_pPartialData, sizeof(unsigned char) * (len + 1));
    this->m_pPartialData[len] = '\0';

    QString str((const char*)this->m_pPartialData);
    SAFE_DELETE(this->m_pPartialData);

    return str;
}
