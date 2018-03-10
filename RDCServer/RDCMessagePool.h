#ifndef RDCMESSAGEPOOL_H
#define RDCMESSAGEPOOL_H


class RDCMessagePool
{
private:
    RDCMessagePool();

private:
    static RDCMessagePool* pool;
};

#endif // RDCMESSAGEPOOL_H
