#include "RDCUtils.h"

#include <QPixmap>
#include <QHostInfo>
#include <QSysInfo>
#include <QScreen>
#include <QDesktopWidget>
#include <QApplication>
#include <zlib.h>

RDCUtils::RDCUtils()
{

}

QString RDCUtils::hostName(void)
{
    return QHostInfo::localHostName();
}

QString RDCUtils::systemVersion(void)
{
    return QSysInfo::prettyProductName();
}

QSize RDCUtils::resolution(void)
{
    return QApplication::primaryScreen()->size();
}

QImage RDCUtils::grabScreen(void)
{
    QPixmap snapShot = QApplication::primaryScreen()->grabWindow(0);
    snapShot = snapShot.scaled(RDCUtils::resolution().width(), RDCUtils::resolution().height());

    return snapShot.toImage();
}

struct ioVec *RDCUtils::compress(const unsigned char* data, int dataLen)
{
    uLong bounds = compressBound(dataLen);

    unsigned char* buff = (unsigned char*)malloc(sizeof(unsigned char) * bounds);
    memset(buff, 0, sizeof(unsigned char) * bounds);

    if(::compress(buff, &bounds, data, dataLen) == Z_OK)
    {
        struct ioVec* dataVec = new struct ioVec();
        dataVec->io_base = buff;
        dataVec->io_compress_length = bounds;

        return dataVec;
    }

    return nullptr;
}

struct ioVec*RDCUtils::uncompress(const unsigned char* data, int dataLen, int oriLen)
{
    uLong oriDataLen = oriLen;
    unsigned char* buff = (unsigned char*)malloc(sizeof(unsigned char) * oriLen);
    memset(buff, 0, sizeof(unsigned char) * oriLen);

    if(::uncompress(buff, &oriDataLen, data, dataLen) == Z_OK)
    {
        struct ioVec* dataVec = new struct ioVec();
        dataVec->io_base = buff;
        dataVec->io_compress_length = oriDataLen;

        return dataVec;
    }

    return nullptr;
}
