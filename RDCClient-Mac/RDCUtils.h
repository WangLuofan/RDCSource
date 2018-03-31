#ifndef RDCUTILS_H
#define RDCUTILS_H

#include <QSize>
#include <QString>
#include <QImage>

typedef struct ioVec
{
    unsigned char* io_base;
    int io_compress_length;
    int io_origin_length;

    ioVec() : io_base(nullptr), io_compress_length(0), io_origin_length(0) {}
    ~ioVec() {free(io_base); io_base = nullptr;}

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
