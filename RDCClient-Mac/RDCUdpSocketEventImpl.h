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

    RDCUdpSocket *pSendSocket(void) const;
    void setSendSocket(RDCUdpSocket *pSendSocket);

private:
    virtual void onScreenDataReceived(RDCUdpSocket*, RDCMessage*);

    void run(void);
    void parseMessage(RDCMessage*);

signals:
    void screen_image_update_signal(QImage);
    void screen_generate_frame_signal(void);

private:
    RDCUdpSocket *m_pSendSocket;        //数据发送Socket
    RDCMessageQueue<RDCMessage*>* m_pMessageQueue;
    bool m_bShouldStopParse;
    struct ioVec* m_pUnCompressedData;
    struct ioVec* m_pCompressedData;
    QSemaphore* m_pSemaphore;
    QSize m_pResolution;
    int m_pOriginData_Length;
};

#endif // RDCUDPSOCKETEVENTIMPL_H
