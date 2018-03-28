#include "RDCClient.h"
#include "RDCHostInfo.h"
#include "RDCTcpSocket.h"
#include "RDCConfiguration.h"
#include "RDCMessage.h"
#include "RDCMessagePool.h"
#include "RDCUtils.h"
#include "RDCUdpSocket.h"
#include "RDCMessageQueue.h"

RDCClient::RDCClient(QObject *parent) : QObject(parent), m_pClientSocket(nullptr),
    m_pScreenDataSocket(nullptr), m_pScreenMsgRecvThread(nullptr),
    m_pSendBuffer(nullptr), m_bIsGeneratingScreenShot(false), m_pMsgQueue(new RDCMessageQueue())
{
    this->m_pSemaphore = new QSemaphore(1);
}

RDCClient::~RDCClient()
{
    delete this->m_pSemaphore;
    delete this->m_pClientSocket;
    delete this->m_pScreenDataSocket;
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
    MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();

    rmsg.get()->setServiceCommand(ServiceCommandSYN);

    QString hostName = RDCUtils::hostName();
    QString sysVer = RDCUtils::systemVersion();

    rmsg.get()->appendChar(hostName.length());
    rmsg.get()->appendString(hostName);
    rmsg.get()->appendChar(sysVer.length());
    rmsg.get()->appendString(sysVer);

    socket->sendMessage(rmsg.get());
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
            //主控端密码验证通过, 建立Udp连接
            if(this->buildUdpConnection())
            {
                //启动接收线程
                if(this->m_pScreenMsgRecvThread == nullptr)
                {
                    this->m_pScreenMsgRecvThread = new RDCScreenMsgReceivedThread(this, this->m_pScreenDataSocket);
                    this->m_pScreenMsgRecvThread->start();
                }
                //发送端口号给主控端
                MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();
                unsigned short localPort = RDCConfiguration::standardConfiguration()
                        ->valueForKey("LocalPort").toInt(nullptr);

                rmsg.get()->setServiceCommand(ServiceCommandConnectionReady);
                rmsg.get()->appendShort(localPort);
                socket->sendMessage(rmsg.get());
            }
        }
            break;
        case ServiceCommandConnectionReady:
        {
            //被控端已经建立连接，连接到被控端，建立新窗口
            unsigned short port = message->nextShort();
            int ipLen = message->nextChar();
            QString ipAddr = message->nextString(ipLen);

            if(this->buildUdpConnection(false, ipAddr.toLatin1().constData(), port))
            {
                //启动接收线程
                if(this->m_pScreenMsgRecvThread == nullptr)
                {
                    this->m_pScreenMsgRecvThread = new RDCScreenMsgReceivedThread(
                                this, this->m_pScreenDataSocket);
                    this->m_pScreenMsgRecvThread->start();
                }

                //发送通知，建立窗口
                emit client_connection_ready_signal(ipAddr);

                //请求屏幕分辨率
                MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();
                rmsg.get()->setServiceCommand(ServiceCommandResolutionRequest);
                this->m_pScreenDataSocket->sendMessage(rmsg.get());
            }
        }
            break;
        case ServiceCommandVerifyFailed:
        {
            //主控端密码验证失败
            emit client_show_message_signal(MessageLevelInfomation, QString("对方密码验证失败"));
        }
            break;
        default:
            break;
    }

    return ;
}

//收到屏幕指令
void RDCClient::onScreenCommandReceived(RDCUdpSocket *, RDCMessage *)
{
    return ;
}

//收到数据指令
void RDCClient::onScreenDataReceived(RDCUdpSocket* socket, RDCMessage* message)
{
    ServiceCommand cmd = message->serviceCommand();
    switch (cmd)
    {
        case ServiceCommandResolutionRequest:
        {
            //主控端请求屏幕分辨率
            QSize reso = RDCUtils::resolution();

            MESSAGE_PTR rmsg = RDCMessagePool::pool()->newMessage();
            rmsg.get()->setServiceCommand(ServiceCommandResolutionResponse);
            rmsg.get()->appendChar(reso.width());
            rmsg.get()->appendChar(reso.height());

            socket->sendMessage(rmsg.get());

            //启动定时器
            emit client_resolution_response_signal();
        }
            break;
        case ServiceCommandResolutionResponse:
        {
            //被控端回复屏幕分辨率
            QSize resolution = QSize(message->nextChar(), message->nextChar());
            //通知窗口，开辟空间
            emit client_resolution_response_signal(resolution);
        }
            break;
        default:
        {
            //放入队列等待解析
        }
            break;
    }
    return ;
}

bool RDCClient::buildUdpConnection(bool bMaster, const char* ipAddr, unsigned short port)
{

    if(this->m_pScreenDataSocket == nullptr)
    {
        this->m_pScreenDataSocket = new RDCUdpSocket();
        this->m_pScreenDataSocket->setSocketEventHandler(this);

        return bMaster ? this->m_pScreenDataSocket->bindAtPort(RDCConfiguration::standardConfiguration()
                                                               ->valueForKey("LocalPort").toInt(nullptr))
                       : this->m_pScreenDataSocket->connectToHost(ipAddr, port);
    }

    return false;
}

void RDCClient::doConnectionSlots(QString token)
{
    //主控端发起连接请求
    MESSAGE_PTR msg = RDCMessagePool::pool()->newMessage();
    msg.get()->setServiceCommand(ServiceCommandConnectionQuery);
    msg.get()->appendChar(token.length());
    msg.get()->appendString(token);

    this->m_pClientSocket->sendMessage(msg.get());
    return ;
}

void RDCClient::doConnectionQuerySlots(bool result)
{
    MESSAGE_PTR msg = RDCMessagePool::pool()->newMessage();
    if(result == true)
    {
        //用户同意建立连接, 查看是否需要密码验证
        bool shouldVerifyPassword = RDCConfiguration::standardConfiguration()
                ->valueForKey("verifyPassword").toBool();
        //发送密码验证请求
        msg.get()->setServiceCommand(ServiceCommandVerifyRequest);
        msg.get()->appendChar(shouldVerifyPassword);
    }
    else
    {
        //用户不同意建立连接
        msg.get()->setServiceCommand(ServiceCommandConnectionDenied);
    }

    this->m_pClientSocket->sendMessage(msg.get());
    return ;
}

void RDCClient::doPasswordVerifyResultSlots(bool bSuccess)
{
    MESSAGE_PTR msg = RDCMessagePool::pool()->newMessage();
    msg.get()->setServiceCommand(bSuccess ? ServiceCommandVerifyComplete : ServiceCommandVerifyFailed);
    this->m_pClientSocket->sendMessage(msg.get());
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
void RDCClient::doScreenGenerate()
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
            int bytes_per_time = 4080, bytes_need_to_send = vec->length, bytes_already_sended = 0,
                    packet_count = (int)(vec->length / bytes_per_time) + 1, packet_index = 0;
            while(bytes_need_to_send > 0)
            {
                //写入命令字
                MESSAGE_PTR msg = RDCMessagePool::pool()->newMessage();
                msg.get()->setServiceCommand(ServiceCommandScreenData);

                //写入是否第一帧
                msg.get()->appendChar(1);

                //写入包索引
                msg.get()->appendChar(++packet_index);

                //写入包总数
                msg.get()->appendChar(packet_count);

                //写入保留位
                msg.get()->appendChar(0);

                //写入总数据长度
                msg.get()->appendInteger(snapShot.sizeInBytes());

                //写入包数据长度
                if(bytes_need_to_send > bytes_per_time)
                {
                    msg.get()->appendInteger(bytes_per_time);

                    //写入数据
                    msg.get()->appendData(vec->data + bytes_already_sended, bytes_per_time);

                    //调整相关值
                    bytes_need_to_send -= bytes_per_time;
                    bytes_already_sended += bytes_per_time;
                }
                else
                {
                    msg.get()->appendInteger(bytes_need_to_send);

                    //写入数据
                    msg.get()->appendData(vec->data + bytes_already_sended, bytes_need_to_send);

                    //调整相关值
                    bytes_need_to_send = 0;
                    bytes_already_sended = vec->length;
                }

                this->m_pSemaphore->release();
                this->m_pMsgQueue->push_back(msg);
            }

            free(vec);
            vec = nullptr;
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

        int index = 0, start = 0;
        for(int i = 0; i < snapShot.sizeInBytes(); i += 4)
        {
            unsigned char R = (*(ptr + i)) ^ (*(this->m_pSendBuffer + i));
            unsigned char G = (*(ptr + i + 1)) ^ (*(this->m_pSendBuffer + i + 1));
            unsigned char B = (*(ptr + i + 2)) ^ (*(this->m_pSendBuffer + i + 2));

            *(diff + index++) = R;
            *(diff + index++) = G;
            *(diff + index++) = B;
        }

        struct ioVec* vec = RDCUtils::compress(diff, diffLen);

        if(vec != nullptr)
        {
            //分包发送
            int bytes_per_time = 4080, bytes_need_to_send = vec->length, bytes_already_sended = 0,
                    packet_count = (int)(vec->length / bytes_per_time) + 1, packet_index = 0;
            while(bytes_need_to_send > 0)
            {
                //写入命令字
                MESSAGE_PTR msg = RDCMessagePool::pool()->newMessage();
                msg.get()->setServiceCommand(ServiceCommandScreenData);

                //写入是否第一帧
                msg.get()->appendChar(0);

                //写入包索引
                msg.get()->appendChar(++packet_index);

                //写入包总数
                msg.get()->appendChar(packet_count);

                //写入保留位
                msg.get()->appendChar(0);

                //写入总数据长度
                msg.get()->appendInteger(diffLen);

                //写入包数据长度
                if(bytes_need_to_send > bytes_per_time)
                {
                    msg.get()->appendInteger(bytes_per_time);

                    //写入数据
                    msg.get()->appendData(vec->data + bytes_already_sended, bytes_per_time);

                    //调整相关值
                    bytes_need_to_send -= bytes_per_time;
                    bytes_already_sended += bytes_per_time;
                }
                else
                {
                    msg.get()->appendInteger(bytes_need_to_send);

                    //写入数据
                    msg.get()->appendData(vec->data + bytes_already_sended, bytes_need_to_send);

                    //调整相关值
                    bytes_need_to_send = 0;
                    bytes_already_sended = vec->length;
                }

                this->m_pSemaphore->release();
                this->m_pMsgQueue->push_back(msg);
            }

            free(vec);
            vec = nullptr;
        }
    }

    //操作执行结束
    this->m_bIsGeneratingScreenShot = false;
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
