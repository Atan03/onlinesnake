#include"socketLib.h"

void sendFd(int sockFd, int fd2Send)
{ 
    iovec iov[1]; 
    msghdr msg;  
    char buff[0];   

    iov[0].iov_base = buff;
    iov[0].iov_len = 1;
    
    msg.msg_name = nullptr;
    msg.msg_namelen = 0;
    
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    
    cmsghdr cm;
    cm.cmsg_len = CMSG_LEN(sizeof(sockFd)); 
    cm.cmsg_level = SOL_SOCKET;         
    cm.cmsg_type = SCM_RIGHTS;          
    *(int*)CMSG_DATA(&cm) = fd2Send; 

    msg.msg_control = &cm;
    msg.msg_controllen = CMSG_LEN(sizeof(sockFd));

    int sendState = sendmsg(sockFd, &msg, 0);
    if(sendState == -1) {
        
    }
}

int recvFd(int sockFd)
{
    iovec iov[1];  
    msghdr msg;
    char buff[0];   

    iov[0].iov_base = buff;
    iov[0].iov_len = 1;
    
    msg.msg_name = nullptr;
    msg.msg_namelen = 0;
    
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    cmsghdr cm;

    msg.msg_control = &cm;
    msg.msg_controllen = CMSG_LEN(sizeof(sockFd));

    int recvState = recvmsg(sockFd, &msg, 0);
    if(recvState == -1) {

    }

    int fd = *(int*)CMSG_DATA(&cm);
    return fd;
}

std::string recvStr(int sockFd, int expRecvLen) {
    char buffer[expRecvLen + 1];

    ctnuRecv(sockFd, buffer, expRecvLen);
    
    return std::string(buffer);
}

void ctnuRecv(int sockFd, char* buffer, int expRecvLen) {
    int curRecvLen = 0;
    while(curRecvLen < expRecvLen) {
        curRecvLen += 
            recv(sockFd, buffer + curRecvLen, expRecvLen - curRecvLen, 0);
        if(curRecvLen == -1) {
            throw std::runtime_error("Receive from sockFd failure. errno: " + std::to_string(errno));
        }
    }
    buffer[expRecvLen] = '\0';
}

char recvChar(int sockFd) {
    char c;
    int recvState = recv(sockFd, &c, sizeof(c), 0);
    if(recvState == -1) {
        throw std::runtime_error("Receive from sockFd failure. errno: " + std::to_string(errno));
    }
    return c;
}

void sendChar(int sockFd, char c) {
    int sendState = send(sockFd, &c, sizeof(c), 0);
    if(sendState == -1) {
        throw std::runtime_error("Send based on sockFd failure. errno: " + std::to_string(errno));
    }
}

int socketErrTrw() {
    int sockFd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockFd == -1) {
        throw std::runtime_error("Socket application failure, errno: " + std::to_string(errno));
    }
    return sockFd;
}

void bindErrTrw(int sockFd, const sockaddr_in &saddr) {
    int bindState = bind(sockFd, (sockaddr *)&saddr, sizeof(saddr));
    char* message = strerror(errno);
    printf("%s\n", message);    
    if(bindState != 0) {
        throw std::runtime_error("Socket binding failure, errno: " + std::to_string(errno));
    }
}

void listenErrTrw(int sockFd, int backLog) {
    int listenState = listen(sockFd, backLog);
    if(listenState != 0) {
        throw std::runtime_error("Connection listen failure errno: " + std::to_string(errno));
    } 
}

void connectErrTrw(int sockFd, const sockaddr_in *saddrPtr) {
    int connectState = connect(sockFd, (sockaddr *)saddrPtr, sizeof(*saddrPtr));
    if(connectState != 0) {
        throw std::runtime_error("Connect failure errno: " + std::to_string(errno));
    }
}

int acceptErrTrw(int sockFd) {
    int acceptedFd = accept(sockFd, nullptr, nullptr);
    if(acceptedFd == -1) {
        throw std::runtime_error("Accept failure errno: " + std::to_string(errno));
    }
    return acceptedFd;
}

sockaddr_in getSockAddr(const char *ipAddr, int portNum) {
    sockaddr_in sockAddr;
    bzero((void*)&sockAddr, sizeof(sockAddr));

    sockAddr.sin_addr.s_addr = inet_addr(ipAddr);
    sockAddr.sin_port = htons(portNum);
    sockAddr.sin_family = AF_INET;

    return sockAddr;
}

sockaddr_in getSockAddr(const std::string &ipAddr, int portNum) {
    return getSockAddr(ipAddr.c_str(), portNum);
}

int makeServerSocket(const char *localIp, int portNum, int backLog) {
    int sockFd = socketErrTrw();

    sockaddr_in sockAddrIn = getSockAddr(localIp, portNum);
    
    bindErrTrw(sockFd, sockAddrIn);

    listenErrTrw(sockFd, backLog);

    return sockFd;
}

int makeServerSocket(const std::string &localIp, int portNum, int backLog) {
    return makeServerSocket(localIp.c_str(), portNum, backLog);
}

int connectToServer(const char *ipAddr, int portNum) {
    int sockFd = socketErrTrw();
    
    sockaddr_in sockAddrIn = getSockAddr(ipAddr, portNum);

    connectErrTrw(sockFd, &sockAddrIn);

    return sockFd;
}

int connectToServer(const std::string &ipAddr, int portNum) {
    return connectToServer(ipAddr.c_str(), portNum);
}