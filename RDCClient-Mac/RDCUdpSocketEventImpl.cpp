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

#define SAFE_DELETE(ptr) if(ptr != nullptr) {free(ptr); ptr = nullptr;}

//收到屏幕指令
RDCUdpSocketEventImpl::RDCUdpSocketEventImpl(QObject* parent) : QThread(parent),
    m_pMessageQueue(new RDCMessageQueue<MESSAGE_PTR>()),
    m_bShouldStopParse(false), m_pCompressedData(nullptr),
    m_pUnCompressedData(nullptr), m_pPacketsReceived(0), m_pSemaphore(new QSemaphore(1))
{
}

void RDCUdpSocketEventImpl::onScreenCommandReceived(RDCUdpSocket *, RDCMessage *)
{
    return ;
}

//收到数据指令,放入队列等待解析
void RDCUdpSocketEventImpl::onScreenDataReceived(RDCUdpSocket*, MESSAGE_PTR message)
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
            MESSAGE_PTR msg = this->m_pMessageQueue->pop_front();

            if(msg != nullptr)
                this->parseMessage(msg.get());
        }
        this->m_pSemaphore->release();
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
    if(cmd == ServiceCommandResolution)
    {
        //请求桌面分辨率
        this->m_pResolution = QSize(msg->nextShort(), msg->nextShort());
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
            this->m_pCompressedData = (unsigned char*)malloc(sizeof(unsigned char) * com_len);
            memset(this->m_pCompressedData, 0, sizeof(unsigned char) * com_len);
        }

        ++this->m_pPacketsReceived;

        //复制图像数据到对应的位置
        const unsigned char* img_data = msg->data() + msg->current();
        memcpy(this->m_pCompressedData +
               (index - 1) * img_data_length, img_data, msg->size() - msg->current());

        //解析到一个完整的数据包
        if(this->m_pPacketsReceived == total)
        {
            //解压缩数据
            struct ioVec* vec = RDCUtils::uncompress(this->m_pCompressedData, com_len, ori_len);
            if(vec != nullptr)
            {
                //解压到数据之后，建立图片
                if(bFirst)
                    this->m_pUnCompressedData = vec;
                else
                {
                    //非第一帧
                    int idx = 0;
                    for(int i = 0; i < this->m_pUnCompressedData->length; i += 4)
                    {
                        (*(this->m_pUnCompressedData->data + i)) ^=
                                (*(this->m_pCompressedData + (idx++)));
                        (*(this->m_pUnCompressedData->data + i + 1)) ^=
                                (*(this->m_pCompressedData + (idx++)));
                        (*(this->m_pUnCompressedData->data + i + 2)) ^=
                                (*(this->m_pCompressedData + (idx++)));
                    }
                }
            }

            QImage image = QImage(this->m_pUnCompressedData->data,
                                       this->m_pResolution.width(), this->m_pResolution.height(),
                                       QImage::Format_ARGB32_Premultiplied);
            if(image.isNull() == false)
            {
                //通知窗口显示图片
                emit screen_image_update_signal(image);
            }

            //清理资源, 重新开始解析下一个数据包
            SAFE_DELETE(this->m_pCompressedData);
            this->m_pPacketsReceived = 0;
        }
    }
    return ;
}
