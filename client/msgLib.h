#ifndef _MSGLIB_H_
#define _MSGLIB_H_

#include"clientUtils.h"
#include"cursesLib.h"

void frameRecvr(int sockFd, MainFrame &mainFrame, HintMsg &hintMsg, int protoLen);

void infoRecvr(int sockFd, MainFrame &mainFrame, HintMsg &hintMsg, int protoLen);

GameState msgRecvr(int sockFd, MainFrame &mainFrame, HintMsg &hintMsg);

void msgSender(int sockFd, const Sig sig, const Action action);

#endif