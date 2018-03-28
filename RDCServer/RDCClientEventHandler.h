#ifndef RDCCLIENTEVENTHANDLER_H
#define RDCCLIENTEVENTHANDLER_H

class RDCTcpSocket;
class RDCMessage;

void onClientConnectedMessageReceived(RDCTcpSocket*, RDCMessage*);
void onConnectionQueryMessageReceived(RDCTcpSocket*, RDCMessage*);
void onConnectionDeniedMessageReceived(RDCTcpSocket*, RDCMessage*);
void onPasswordVerifyMessageReceived(RDCTcpSocket*, RDCMessage*);
void onPasswordVerifyResultMessageReceived(RDCTcpSocket*, RDCMessage*);
void onConnectionReadyMessageReceived(RDCTcpSocket*, RDCMessage*);

#endif // RDCCLIENTEVENTHANDLER_H
