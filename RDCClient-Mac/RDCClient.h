#ifndef RDCCLIENT_H
#define RDCCLIENT_H

#include "RDCScreenMsgReceivedThread.h"
#include "RDCTcpSocketEventHandler.h"
#include "RDCUdpSocketEventHandler.h"
#include "RDCMessageQueue.h"

#include <QObject>
#include <QSize>
#include <QString>
#include <QSemaphore>
#include <QImage>

class RDCTcpSocket;
class RDCUdpSocket;
class RDCMessage;
class RDCUdpSocketEventImpl;
class RDCScreenDataSendThread;
class RDCClient : public QObject, private RDCTcpSocketEventHandler
{
    Q_OBJECT
public:
    typedef enum
    {
        MessageLevelInfomation,
        MessageLevelQuestion,
        MessageLevelWarning,
        MessageLevelCritical
    }MessageLevel;

public:
    explicit RDCClient(QObject* = nullptr);
    ~RDCClient();

private:
    virtual void onErrorOccured(RDCTcpSocket*, const char*);
    virtual void onClientConnected(RDCTcpSocket*, RDCHostInfo*);
    virtual void onConnectedToServer(RDCTcpSocket*);
    virtual void onClientDisconnected(RDCTcpSocket*);
    virtual void onTimeOutEventOccured(RDCTcpSocket*);
    virtual void onMessageReceived(RDCTcpSocket*, RDCMessage *);

    bool buildUdpConnection(RDCUdpSocket*&, unsigned short = 0, bool = true, const char* = nullptr);

signals:
    void client_info_update_signal(QString, QString);
    void client_show_message_signal(int, QString);
    void client_verify_password_signal(QString);
    void client_connection_ready_signal(QString);
    void client_should_update_screen_image(QImage);

private slots:
    void doConnectionSlots(QString);
    void doConnectionQuerySlots(bool);
    void doPasswordVerifyResultSlots(bool);
    void doScreenGenerate(void);
    void doUpdateScreenImageSlots(QImage);

public slots:
    void Start(void);
    void Stop(void);

private:
    RDCTcpSocket* m_pClientSocket;
    RDCUdpSocket* m_pScreenDataSocket;
    RDCUdpSocket* m_pScreenCommandSocket;
    RDCMessageQueue<RDCMessage*>* m_pMsgQueue;
    RDCMessageQueue<QImage*>* m_pImageQueue;
    RDCScreenMsgReceivedThread* m_pScreenMsgRecvThread;
    RDCScreenDataSendThread* m_pScreenDataSendThread;
    unsigned char* m_pSendBuffer;
    QSemaphore* m_pMsgQueueSemaphore;
    QSemaphore* m_pImageQueueSemaphore;
    RDCUdpSocketEventImpl* m_pUdpSocketEventImpl;
};

#endif // RDCCLIENT_H
