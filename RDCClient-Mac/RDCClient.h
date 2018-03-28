#ifndef RDCCLIENT_H
#define RDCCLIENT_H

#include "RDCScreenMsgReceivedThread.h"
#include "RDCTcpSocketEventHandler.h"
#include "RDCUdpSocketEventHandler.h"

#include <QObject>
#include <QSize>
#include <QSemaphore>

class RDCTcpSocket;
class RDCUdpSocket;
class RDCMessageQueue;
class RDCClient : public QObject, private RDCTcpSocketEventHandler, private RDCUdpSocketEventHandler
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
    explicit RDCClient(QObject *parent = nullptr);
    ~RDCClient();

private:
    virtual void onErrorOccured(RDCTcpSocket*, const char*);
    virtual void onClientConnected(RDCTcpSocket*, RDCHostInfo*);
    virtual void onConnectedToServer(RDCTcpSocket*);
    virtual void onClientDisconnected(RDCTcpSocket*);
    virtual void onTimeOutEventOccured(RDCTcpSocket*);
    virtual void onMessageReceived(RDCTcpSocket*, RDCMessage *);

    virtual void onScreenCommandReceived(RDCUdpSocket*, RDCMessage*);
    virtual void onScreenDataReceived(RDCUdpSocket*, RDCMessage*);

    bool buildUdpConnection(bool = true, const char* = nullptr, unsigned short = 0);

signals:
    void client_info_update_signal(QString, QString);
    void client_show_message_signal(int, QString);
    void client_verify_password_signal(QString);
    void client_connection_ready_signal(QString);
    void client_resolution_response_signal(QSize = QSize());

private slots:
    void doConnectionSlots(QString);
    void doConnectionQuerySlots(bool);
    void doPasswordVerifyResultSlots(bool);
    void doScreenGenerate(void);

public slots:
    void Start(void);
    void Stop(void);

private:
    RDCTcpSocket* m_pClientSocket;
    RDCUdpSocket* m_pScreenDataSocket;
    RDCMessageQueue* m_pMsgQueue;
    RDCScreenMsgReceivedThread* m_pScreenMsgRecvThread;
    unsigned char* m_pSendBuffer;
    bool m_bIsGeneratingScreenShot;
    QSemaphore* m_pSemaphore;
};

#endif // RDCCLIENT_H
