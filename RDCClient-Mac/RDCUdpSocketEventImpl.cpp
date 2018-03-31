#include "RDCUdpSocketEventImpl.h"
#include "RDCMessageQueue.h"
#include "RDCUdpSocket.h"
#include "RDCMessage.h"
#include "RDCUtils.h"

#include <QSize>
#include <QImage>
#include <QDebug>
#include <QTime>
#include <QSemaphore>

#define SAFE_DELETE(ptr) if(ptr != nullptr) {delete ptr; ptr = nullptr;}

//收到屏幕指令
RDCUdpSocketEventImpl::RDCUdpSocketEventImpl(QObject* parent) : QThread(parent),
    m_pMessageQueue(new RDCMessageQueue<RDCMessage*>()),
    m_bShouldStopParse(false), m_pCompressedData(nullptr),
    m_pUnCompressedData(nullptr), m_pSemaphore(new QSemaphore(1))
{
}

RDCUdpSocketEventImpl::~RDCUdpSocketEventImpl()
{
    SAFE_DELETE(this->m_pCompressedData);
    SAFE_DELETE(this->m_pUnCompressedData);
}

//收到数据指令,放入队列等待解析
void RDCUdpSocketEventImpl::onScreenDataReceived(RDCUdpSocket*, RDCMessage *message)
{
    this->m_pSemaphore->acquire();
    this->m_pMessageQueue->push_back(message);
    this->m_pSemaphore->release();
    return ;
}

void RDCUdpSocketEventImpl::run(void)
{
    while(this->m_bShouldStopParse == false)
    {
        this->m_pSemaphore->acquire();
        if(this->m_pMessageQueue->empty() == false)
        {
            RDCMessage* msg = this->m_pMessageQueue->pop_front();

            if(msg != nullptr)
                this->parseMessage(msg);
        }
        this->m_pSemaphore->release();
        QThread::msleep(1);
    }
    return ;
}


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
void RDCUdpSocketEventImpl::parseMessage(RDCMessage *msg)
{
    //确认是屏幕数据帧
    ServiceCommand cmd = msg->serviceCommand();
    if(cmd == ServiceCommandTellResolution)
    {
        //请求桌面分辨率
        this->m_pResolution = QSize(msg->nextShort(), msg->nextShort());

        //请求屏幕第一帧
        RDCMessage* msg = RDCMessage::newMessage(ServiceCommandScreenFirstFrame);
        this->m_pSendSocket->sendMessage(msg);
    }else if(cmd == ServiceCommandScreenFirstFrame || cmd == ServiceCommandScreenNextFrame)
    {
        //主控端请求屏幕数据, 通知Client产生图像
        emit screen_generate_frame_signal();
    }
    else if(cmd == ServiceCommandScreenData)
    {
        int img_data_length = 5104;
        bool bFirst = (bool)msg->nextChar();        //是否屏幕第一帧
        int index = msg->nextChar();                //当前包索引
        int total = msg->nextChar();                //总包数
        msg->nextChar();                            //跳过保留字段
        int ori_len = msg->nextInteger();           //原始数据总长度
        int com_len = msg->nextInteger();           //压缩数据总长度

        //如果未开辟空间，则先申请内存
        if(this->m_pCompressedData == nullptr)
        {
            this->m_pCompressedData = new struct ioVec();

            this->m_pCompressedData->io_base = (unsigned char*)malloc(sizeof(unsigned char) * com_len);
            this->m_pCompressedData->io_compress_length = com_len;
            this->m_pCompressedData->io_origin_length = ori_len;

            memset(this->m_pCompressedData->io_base, 0, sizeof(unsigned char) * com_len);
        }

        //复制图像数据到对应的位置
        const unsigned char* img_data = msg->data() + msg->current();
        memcpy(this->m_pCompressedData->io_base +
               index * img_data_length, img_data, msg->size() - msg->current());
    }
    else if(cmd == ServiceCommandOneFrameEndNotification)
    {
        //一帧图像已经传输结束
        struct ioVec* vec = RDCUtils::uncompress(this->m_pCompressedData->io_base,
                                                 this->m_pCompressedData->io_compress_length,
                                                 this->m_pCompressedData->io_origin_length);
        if(vec != nullptr)
        {
            //解压到数据之后，建立图片
            if(this->m_pUnCompressedData == nullptr)
                this->m_pUnCompressedData = vec;
            else
            {
                //非第一帧
                int idx = 0;
                for(int i = 0; i < this->m_pUnCompressedData->io_compress_length; i += 4)
                {
                    (*(this->m_pUnCompressedData->io_base + i)) ^=
                            (*(vec->io_base + (idx++)));
                    (*(this->m_pUnCompressedData->io_base + i + 1)) ^=
                            (*(vec->io_base + (idx++)));
                    (*(this->m_pUnCompressedData->io_base + i + 2)) ^=
                            (*(vec->io_base + (idx++)));
                }
                delete vec;
            }

            QImage image = QImage(this->m_pUnCompressedData->io_base,
                                  this->m_pResolution.width(), this->m_pResolution.height(),
                                  QImage::Format_ARGB32_Premultiplied);
            if(image.isNull() == false)
            {
                //通知窗口显示图片
                emit screen_image_update_signal(image);
            }
        }

        SAFE_DELETE(this->m_pCompressedData);

        RDCMessage* nxtMsg = RDCMessage::newMessage(ServiceCommandScreenNextFrame);
        this->m_pSendSocket->sendMessage(nxtMsg);
    }

    delete msg;
    return ;
}

RDCUdpSocket *RDCUdpSocketEventImpl::pSendSocket(void) const
{
    return this->m_pSendSocket;
}

void RDCUdpSocketEventImpl::setSendSocket(RDCUdpSocket *pSendSocket)
{
    this->m_pSendSocket = pSendSocket;
    return ;
}
