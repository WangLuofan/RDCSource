#ifndef RDCUDPSOCKETEVENTIMPL_H
#define RDCUDPSOCKETEVENTIMPL_H

#include "RDCMessageQueue.h"
#include "RDCUdpSocketEventHandler.h"

#include <QThread>
#include <QSize>
#include <QImage>

struct ioVec;
class QSemaphore;
class RDCUdpSocket;
class RDCMessage;
class RDCScreenWindow;
class RDCUdpSocketEventImpl : public QThread, private RDCUdpSocketEventHandler
{
    Q_OBJECT
public:
    explicit RDCUdpSocketEventImpl(QObject* = nullptr);
    ~RDCUdpSocketEventImpl();

private:
    virtual void onScreenCommandReceived(RDCUdpSocket*, RDCMessage*);
    virtual void onScreenDataReceived(RDCUdpSocket*, RDCMessage*);

    void run(void);
    void parseMessage(RDCMessage*);

signals:
    void screen_image_update_signal(QImage);

private:
    RDCMessageQueue<RDCMessage*>* m_pMessageQueue;
    bool m_bShouldStopParse;
    struct ioVec* m_pUnCompressedData;
    unsigned char* m_pCompressedData;
    int m_pPacketsParsed;
    QSemaphore* m_pSemaphore;
    QSize m_pResolution;
};

#endif // RDCUDPSOCKETEVENTIMPL_H
