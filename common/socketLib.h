#ifndef _SOCKETLIB_H_
#define _SOCKETLIB_H_

#include<string>
#include<stdexcept>
#include<cstdio>
#include<cstring>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<time.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include"../protoFile/protoMsg.pb.h"

// const int HOSTLEN = 256, BACKLOG = 6;
const int DFLPORT = 10312, DFLBACKLOG = 6, BUFFERSIZE = 65536, PROTOLENSIZE = 5;

/* Send file descriptor and context with UNIX socket */
void sendFd(int sockFd, int fd2Send);
/* Receive file descriptor and context with UNIX socket */
int recvFd(int sockFd);

/* Receive a string of given length from socket */
std::string recvStr(int sockFd, int expRecvLen);
/* Continously receive char from socket to given buffer */
void ctnuRecv(int sockFd, char* buffer, int expRecvLen);
/* Receive a char from socket */
char recvChar(int sockFd);

/* Receive char stream of given length and parse it to given protobuf */
template<typename T>
void recvProtoMsg(int sockFd, int protoLen, T &protoMsg) {
    char buffer[protoLen + 1];

    ctnuRecv(sockFd, buffer, protoLen);

    protoMsg.ParseFromArray(buffer, protoLen + 1);
}

/* Send a char to socket */
void sendChar(int sockFd, char c);

/* Send given protobuf's serialized data to sockFd with its length */
template<typename T>
void sendProtoMsg(int sockFd, const T &protoMsg) {
    char buffer[BUFFERSIZE], digitBuf[PROTOLENSIZE + 1];
    bzero(buffer, BUFFERSIZE);  bzero(digitBuf, PROTOLENSIZE + 1);

    protoMsg.SerializeToArray(buffer, BUFFERSIZE);
    std::string bufferLen(std::to_string(protoMsg.ByteSizeLong()));
    strcpy(digitBuf, bufferLen.c_str());
    
    int sendState = send(sockFd, digitBuf, PROTOLENSIZE, 0);
    if(sendState == -1) {
        throw std::runtime_error("send based on sockFd failure. (digitBuf)");
    }
    sendState = send(sockFd, buffer, protoMsg.ByteSizeLong(), 0);
    if(sendState == -1) {
        throw std::runtime_error("send based on sockFd failure. (protoBuf)");
    }
}

/* Establish socket, return sockFd */
int socketErrTrw();

/* Bind given sockFd with saddr's configuration */
void bindErrTrw(int sockFd, const sockaddr_in &saddr);

/* Listen sockFd's connection */
void listenErrTrw(int sockFd, int backLog);

/* Bind given sockFd to saddr's configuration */
void connectErrTrw(int sockFd, const sockaddr_in *saddrPtr);

/* Accept corresponding sockFd's connection request , return new socket */
int acceptErrTrw(int sockFd);

/* Get corresponding socaddr with ipAddr and portNum */
sockaddr_in getSockAddr(const char *ipAddr, int portNum);
/* Get corresponding socaddr with ipAddr and portNum */
sockaddr_in getSockAddr(const std::string &ipAddr, int portNum);

/* Appply a socket, listen it on portNum with max connection number of backLog
   returning socket*/
int makeServerSocket(const char *localIp, int portNum, int backLog);
/* Appply a socket, listen it on portNum with max connection number of backLog
   returning socket*/
int makeServerSocket(const std::string &localIp, int portNum, int backLog);

/* Apply a socket then connect socket<ipAddr, portNum> returning socket*/
int connectToServer(const char *ipAddr, int portNum);
/* Apply a socket then connect socket<ipAddr, portNum> returning socket*/
int connectToServer(const std::string &ipAddr, int portNum);

#endif