#ifndef RDCUTILS_H
#define RDCUTILS_H

#include <QSize>
#include <QString>
#include <QImage>

typedef struct ioVec
{
    unsigned char* data;
    int length;

    ~ioVec() {free(data); data = nullptr;}

}ioVec;

class RDCUtils
{
public:
    RDCUtils();
    static QString hostName(void);
    static QString systemVersion(void);
    static QSize resolution(void);
    static QImage grabScreen(void);
    static struct ioVec* compress(const unsigned char*, int);
    static struct ioVec* uncompress(const unsigned char*, int, int);
};

#endif // RDCUTILS_H
