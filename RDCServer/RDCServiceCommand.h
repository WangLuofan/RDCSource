#ifndef RDCSERVICECOMMAND_H
#define RDCSERVICECOMMAND_H

typedef enum
{
    ServiceCommandUnknown = 'SCUK',
    ServiceCommandSYN = 'SCSN',                      //上线通知
    ServiceCommandACK = 'SCAK',                      //上线确认
    ServiceCommandConnectionQuery = 'SCCQ',          //连接查询
    ServiceCommandConnectionResponse = 'SCCR',       //连接请求
    ServiceCommandTokenNotFound = 'STNF',            //未注册的Token
    ServiceCommandConnectionDenied = 'SCCD',         //被控端拒绝连接
    ServiceCommandVerifyRequest = 'SCVR',            //被控端要求密码验证
    ServiceCommandVerifyComplete = 'SCVC',           //主控端验证通过
    ServiceCommandVerifyFailed = 'SCVF',             //主控端验证失败
    ServiceCommandConnectionReady = 'SCRY',          //被控端已准备好连接
    ServiceCommandResolutionRequest = 'RREQ',        //请求桌面分辨率
    ServiceCommandResolutionResponse = 'RRES',       //回应桌面分辨率
    ServiceCommandScreenFirstFrame = 'SFRM',         //主控端请求屏幕第一帧
    ServiceCommandScreenData = 'SCSD',               //被控端发送的屏幕数据
    ServiceCommandScreenDataNone = 'SCSN',           //无屏幕差异数据
    ServiceCommandScreenNextFrame = 'SNFR',          //主控端请求下一帧屏幕
    ServiceCommandConnectionClosed = 'SCCC',         //主/被控端连接断开
    ServiceCommandApplicationQuit = 'SCAQ',          //应用退出
}ServiceCommand;

#endif // RDCSERVICECOMMAND_H
