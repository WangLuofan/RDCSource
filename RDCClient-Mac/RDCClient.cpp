#include "RDCClient.h"
#include "RDCHostInfo.h"
#include "RDCTcpSocket.h"
#include "RDCConfiguration.h"
#include "RDCMessage.h"
#include "RDCUtils.h"
#include "RDCUdpSocket.h"
#include "RDCMessageQueue.h"
#include "RDCUdpSocketEventImpl.h"
#include "RDCScreenDataSendThread.h"

#include <QDebug>

RDCClient::RDCClient(QObject *parent) : QObject(parent), m_pClientSocket(nullptr),
    m_pScreenDataSocket(nullptr), m_pScreenMsgRecvThread(nullptr),
    m_pSendBuffer(nullptr), m_bIsGeneratingScreenShot(false), m_pMsgQueue(new RDCMessageQueue<RDCMessage*>()),
    m_pUdpSocketEventImpl(new RDCUdpSocketEventImpl(this)), m_pScreenDataSendThread(nullptr),
    m_pImageQueue(new RDCMessageQueue<QImage*>())
{
    this->m_pMsgQueueSemaphore = new QSemaphore(1);
    this->m_pImageQueueSemaphore = new QSemaphore(1);

    QObject::connect(this->m_pUdpSocketEventImpl, SIGNAL(screen_image_update_signal(QImage)),
                     this, SLOT(doUpdateScreenImageSlots(QImage)), Qt::DirectConnection);

    //启动数据解析
    this->m_pUdpSocketEventImpl->start();
}

RDCClient::~RDCClient()
{
    free(this->m_pSendBuffer);
    delete this->m_pMsgQueueSemaphore;
    delete this->m_pImageQueueSemaphore;
    delete this->m_pUdpSocketEventImpl;
    delete this->m_pClientSocket;
    delete this->m_pScreenDataSocket;
    delete this->m_pImageQueue;
    delete this->m_pMsgQueue;
}

void RDCClient::onErrorOccured(RDCTcpSocket *, const char* errmsg)
{
    emit client_show_message_signal(MessageLevelCritical, QString(errmsg));
    return ;
}

void RDCClient::onClientConnected(RDCTcpSocket* socket, RDCHostInfo *)
{
    return ;
}

void RDCClient::onConnectedToServer(RDCTcpSocket* socket)
{
    //客户端连接到服务器, 发送主机名，系统版本信息
    RDCMessage* rmsg = RDCMessage::newMessage(ServiceCommandSYN);

    QString hostName = RDCUtils::hostName();
    QString sysVer = RDCUtils::systemVersion();

    rmsg->appendChar(hostName.length());
    rmsg->appendString(hostName);
    rmsg->appendChar(sysVer.length());
    rmsg->appendString(sysVer);

    socket->sendMessage(rmsg);

    delete rmsg;
    return ;
}

void RDCClient::onClientDisconnected(RDCTcpSocket *)
{
    return ;
}

void RDCClient::onTimeOutEventOccured(RDCTcpSocket *)
{
    return ;
}

void RDCClient::onMessageReceived(RDCTcpSocket* socket, RDCMessage* message)
{
    ServiceCommand cmd = message->serviceCommand();

    switch (cmd)
    {
        case ServiceCommandACK:
        {
            //取出Token及Password
            int tokenLen = message->nextChar();
            QString token = message->nextString(tokenLen);

            int passLen = message->nextChar();
            QString password = message->nextString(passLen);

            emit client_info_update_signal(token, password);
        }
            break;
        case ServiceCommandTokenNotFound:
        {
            //未找到该Token
            QString prompt = QString("未注册的Token, 请重新输入.");
            emit client_show_message_signal(2, prompt);
        }
            break;
        case ServiceCommandConnectionQuery:
        {
            //收到主控端的连接请求，获取HostName及IP地址, 询问用户
            int hostLen = message->nextChar();
            QString hostName = message->nextString(hostLen);

            int ipLen = message->nextChar();
            QString ipAddr = message->nextString(ipLen);

            bool bShouldAutoAgree = RDCConfiguration::standardConfiguration()
                    ->valueForKey("autoAgreeConn").toBool();
            if(bShouldAutoAgree)
            {
                //如果用户选择自动同意
                this->doConnectionQuerySlots(true);
            }
            else
            {
                //显示对话框询问用户
                QString prompt = QString("是否同意来自%1(%2)的连接请求?").arg(hostName).arg(ipAddr);
                emit client_show_message_signal(MessageLevelQuestion, prompt);
            }
        }
            break;
        case ServiceCommandConnectionDenied:
        {
            //被控端拒绝建立连接, 通知用户
            emit client_show_message_signal(MessageLevelInfomation, QString("对方已拒绝和您建立连接"));
        }
            break;
        case ServiceCommandVerifyRequest:
        {
            //被控端要求密码验证, 显示密码框，进行验证
            int passLen = message->nextChar();
            QString password = message->nextString(passLen);

            emit client_verify_password_signal(password);
        }
            break;
        case ServiceCommandVerifyComplete:
        {
            //主控端密码验证成功，发起Udp连接请求
            RDCMessage* rmsg = RDCMessage::newMessage(ServiceCommandConnectionRequest);
            socket->sendMessage(rmsg);
            delete rmsg;
        }
            break;
        case ServiceCommandConnectionReady:
        {
            //主控端已经建立连接
            unsigned short port = message->nextShort();
            int ipLen = message->nextChar();
            QString ipAddr = message->nextString(ipLen);

            if(this->buildUdpConnection(false, ipAddr.toLatin1().constData(), port))
            {
                //成功连接到主控端, 启动数据发送线程
                if(this->m_pScreenDataSendThread == nullptr)
                {
                    this->m_pScreenDataSendThread = new RDCScreenDataSendThread(
                                this, this->m_pMsgQueueSemaphore, this->m_pMsgQueue, this->m_pScreenDataSocket);
                    this->m_pScreenDataSendThread->start();
                }

                //发送桌面分辨率
                QSize reso = RDCUtils::resolution();
                RDCMessage* rmsg = RDCMessage::newMessage(ServiceCommandTellResolution);
                rmsg->appendShort(reso.width());
                rmsg->appendShort(reso.height());
                this->m_pScreenDataSocket->sendMessage(rmsg);
                delete rmsg;

                //启动定时器,发送屏幕数据
                emit client_screen_data_send_begin_signal();
            }
        }
            break;
        case ServiceCommandVerifyFailed:
        {
            //主控端密码验证失败
            emit client_show_message_signal(MessageLevelInfomation, QString("对方密码验证失败"));
        }
            break;
        case ServiceCommandConnectionRequest:
        {
            int ipLen = message->nextChar();
            QString ipAddr = message->nextString(ipLen);

            //被控端发起连接请求， 建立Udp连接
            if(this->buildUdpConnection())
            {
                if(this->m_pScreenMsgRecvThread == nullptr)
                {
                    //启动数据接收线程
                    this->m_pScreenMsgRecvThread =
                            new RDCScreenMsgReceivedThread(this, this->m_pScreenDataSocket);
                    this->m_pScreenMsgRecvThread->start();
                }

                //发送通知，建立窗口
                emit client_connection_ready_signal(ipAddr);

                //上传端口号
                unsigned short port = RDCConfiguration::standardConfiguration()
                        ->valueForKey("LocalPort").toInt(nullptr);
                RDCMessage* rmsg = RDCMessage::newMessage(ServiceCommandConnectionReady);
                rmsg->appendShort(port);
                socket->sendMessage(rmsg);

                delete rmsg;
            }
        }
            break;
        default:
            break;
    }

    return ;
}

bool RDCClient::buildUdpConnection(bool bMaster, const char* ipAddr, unsigned short port)
{

    if(this->m_pScreenDataSocket == nullptr)
    {
        this->m_pScreenDataSocket = new RDCUdpSocket();
        this->m_pScreenDataSocket->setSocketEventHandler(
                    (RDCUdpSocketEventHandler*)this->m_pUdpSocketEventImpl);

        return bMaster ? this->m_pScreenDataSocket->bindAtPort(RDCConfiguration::standardConfiguration()
                                                               ->valueForKey("LocalPort").toInt(nullptr))
                       : this->m_pScreenDataSocket->connectToHost(ipAddr, port);
    }

    return false;
}

void RDCClient::doConnectionSlots(QString token)
{
    //主控端发起连接请求
    RDCMessage* msg = RDCMessage::newMessage(ServiceCommandConnectionQuery);
    msg->appendChar(token.length());
    msg->appendString(token);

    this->m_pClientSocket->sendMessage(msg);
    delete msg;
    return ;
}

void RDCClient::doConnectionQuerySlots(bool result)
{
    RDCMessage* msg = RDCMessage::newMessage();
    if(result == true)
    {
        //用户同意建立连接, 查看是否需要密码验证
        bool shouldVerifyPassword = RDCConfiguration::standardConfiguration()
                ->valueForKey("verifyPassword").toBool();
        //发送密码验证请求
        msg->setServiceCommand(ServiceCommandVerifyRequest);
        msg->appendChar(shouldVerifyPassword);
    }
    else
    {
        //用户不同意建立连接
        msg->setServiceCommand(ServiceCommandConnectionDenied);
    }

    this->m_pClientSocket->sendMessage(msg);
    delete msg;
    return ;
}

void RDCClient::doPasswordVerifyResultSlots(bool bSuccess)
{
    RDCMessage* msg = RDCMessage::newMessage(
                bSuccess ? ServiceCommandVerifyComplete : ServiceCommandVerifyFailed);
    this->m_pClientSocket->sendMessage(msg);
    delete msg;
    return ;
}

//产生屏幕数据


//++++++++++++++++++++++++++++++++++
//+                                +
//+             cmd                +
//++++++++++++++++++++++++++++++++++
//+    first     +      index      +
//++++++++++++++++++++++++++++++++++
//+    total     +     reserved    +
//++++++++++++++++++++++++++++++++++
//+         origin length          +
//+                                +
//++++++++++++++++++++++++++++++++++
//+          data length           +
//+                                +
//++++++++++++++++++++++++++++++++++
//+         image data             +
//++++++++++++++++++++++++++++++++++

// header_size = 32 + 16 + 16 + 32 + 32 = 16 bytes
void RDCClient::doScreenGenerate(void)
{
    if(this->m_bIsGeneratingScreenShot == true)
        return ;

    //当前正在执行操作
    this->m_bIsGeneratingScreenShot = true;

    QImage snapShot = RDCUtils::grabScreen();
    if(this->m_pSendBuffer == nullptr)
    {
        //第一帧
        this->m_pSendBuffer = (unsigned char*)malloc(sizeof(unsigned char) * snapShot.sizeInBytes());
        memcpy(this->m_pSendBuffer, snapShot.bits(), sizeof(unsigned char) * snapShot.sizeInBytes());

        struct ioVec* vec = RDCUtils::compress(this->m_pSendBuffer,
                                               sizeof(unsigned char) * snapShot.sizeInBytes());
        if(vec != nullptr)
        {
            //分包发送
            int bytes_per_time = 5104, bytes_need_to_send = vec->length, bytes_already_sended = 0,
                    packet_count = (int)(vec->length / bytes_per_time) + 1, packet_index = 1;
            while(bytes_need_to_send > 0)
            {
                //写入命令字
                RDCMessage* msg = RDCMessage::newMessage(ServiceCommandScreenData);

                //写入是否第一帧
                msg->appendChar(1);

                //写入包索引
                msg->appendChar(packet_index++);

                //写入包总数
                msg->appendChar(packet_count);

                //写入保留位
                msg->appendChar(0);

                //写入总数据长度
                msg->appendInteger(snapShot.sizeInBytes());

                //写入压缩数据长度
                 msg->appendInteger(vec->length);

                //写入包数据长度
                if(bytes_need_to_send > bytes_per_time)
                {
                    //写入数据
                    msg->appendData(vec->data + bytes_already_sended, bytes_per_time);

                    //调整相关值
                    bytes_need_to_send -= bytes_per_time;
                    bytes_already_sended += bytes_per_time;
                }
                else
                {
                    //写入数据
                    msg->appendData(vec->data + bytes_already_sended, bytes_need_to_send);

                    //调整相关值
                    bytes_need_to_send = 0;
                    bytes_already_sended = vec->length;
                }

                this->m_pMsgQueue->push_back(msg);
                this->m_pMsgQueueSemaphore->release();
            }

            delete vec;
        }
    }
    else
    {
        //非第一帧
        //比较图像差异， 去除alpha信息
        int diffLen = snapShot.sizeInBytes() * 3 / 4;
        unsigned char* diff = (unsigned char*)malloc(sizeof(unsigned char) * diffLen);
        memset(diff, 0, sizeof(unsigned char) * diffLen);

        unsigned char* ptr = snapShot.bits();

        int index = 0;
        for(int i = 0; i < snapShot.sizeInBytes(); i += 4)
        {
            unsigned char R = (*(ptr + i)) ^ (*(this->m_pSendBuffer + i));
            unsigned char G = (*(ptr + i + 1)) ^ (*(this->m_pSendBuffer + i + 1));
            unsigned char B = (*(ptr + i + 2)) ^ (*(this->m_pSendBuffer + i + 2));

            *(diff + (index++)) = R;
            *(diff + (index++)) = G;
            *(diff + (index++)) = B;
        }

        struct ioVec* vec = RDCUtils::compress(diff, diffLen);
        free(diff);

        if(vec != nullptr)
        {
            //分包发送
            int bytes_per_time = 5104, bytes_need_to_send = vec->length, bytes_already_sended = 0,
                    packet_count = (int)(vec->length / bytes_per_time) + 1, packet_index = 1;
            while(bytes_need_to_send > 0)
            {
                //写入命令字
                RDCMessage* msg = RDCMessage::newMessage(ServiceCommandScreenData);

                //写入是否第一帧
                msg->appendChar(0);

                //写入包索引
                msg->appendChar(packet_index++);

                //写入包总数
                msg->appendChar(packet_count);

                //写入保留位
                msg->appendChar(0);

                //写入总数据长度
                msg->appendInteger(diffLen);

                //写入压缩数据长度
                msg->appendInteger(vec->length);

                //写入包数据长度
                if(bytes_need_to_send > bytes_per_time)
                {
                    //写入数据
                    msg->appendData(vec->data + bytes_already_sended, bytes_per_time);

                    //调整相关值
                    bytes_need_to_send -= bytes_per_time;
                    bytes_already_sended += bytes_per_time;
                }
                else
                {
                    //写入数据
                    msg->appendData(vec->data + bytes_already_sended, bytes_need_to_send);

                    //调整相关值
                    bytes_need_to_send = 0;
                    bytes_already_sended = vec->length;
                }

                this->m_pMsgQueue->push_back(msg);
                this->m_pMsgQueueSemaphore->release();
            }

            delete vec;
        }

        //替换原来的图像数据
        memcpy(this->m_pSendBuffer, snapShot.bits(), sizeof(unsigned char) * snapShot.sizeInBytes());
    }

    //操作执行结束
    this->m_bIsGeneratingScreenShot = false;
    return ;
}

void RDCClient::doUpdateScreenImageSlots(QImage image)
{
    emit client_should_update_screen_image(image);
    return ;
}

void RDCClient::Start()
{
    if(this->m_pClientSocket == nullptr)
    {
        this->m_pClientSocket = new RDCTcpSocket();
        this->m_pClientSocket->setEventHandler(this);
    }

    RDCHostInfo hostInfo(RDCConfiguration::standardConfiguration()->valueForKey("ServerAddr").toString(),
                                            RDCConfiguration::standardConfiguration()->
                                               valueForKey("ServerPort").toInt());
    this->m_pClientSocket->ConnectToHost(&hostInfo);
    return ;
}

void RDCClient::Stop()
{
    if(this->m_pClientSocket != nullptr)
    {
        this->m_pClientSocket->Close();
        this->m_pClientSocket->ExitEventLoop();
    }
    return ;
}
