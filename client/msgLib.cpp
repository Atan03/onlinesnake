#include"msgLib.h"

void frameRecvr(int sockFd, MainFrame &mainFrame, HintMsg &hintMsg, int protoLen) {
    mainFrame.clear();

    myProto::FrameMsg frameMsg;
    recvProtoMsg(sockFd, protoLen, frameMsg);

    for(int i = 0 ; i < frameMsg.pixels_size() ; ++i) {
        mainFrame.emplace_back(frameMsg.pixels(i).x_loc(), frameMsg.pixels(i).y_loc(), 
            frameMsg.pixels(i).symbol());
    }

    if(frameMsg.has_hint_msg()) {
        hintMsg = HintMsg(frameMsg.hint_msg(), DFLHINTDUR);
    }
}

void infoRecvr(int sockFd, MainFrame &mainFrame, HintMsg &hintMsg, int protoLen) {
    myProto::StrMsg strMsg;
    recvProtoMsg(sockFd, protoLen, strMsg);
    frameFromStr(mainFrame, strMsg.main_msg());

    if(strMsg.has_hint_msg()) {
        hintMsg = HintMsg(strMsg.hint_msg(), DFLHINTDUR);
    }
}

GameState msgRecvr(int sockFd, MainFrame &mainFrame, HintMsg &hintMsg) {
    Sig sig = static_cast<Sig>(recvChar(sockFd));
    
    if(sig == Sig::GameOver) {
        GameState gameState = static_cast<GameState>(recvChar(sockFd));
        return gameState;
    }
    else {
        int protoLen = std::stoi(recvStr(sockFd, PROTOLENSIZE));
        switch(sig) {
            case Sig::Frame:
                frameRecvr(sockFd, mainFrame, hintMsg, protoLen);
                cursesRender(mainFrame, hintMsg);
                break;

            case Sig::Info:
                infoRecvr(sockFd, mainFrame, hintMsg, protoLen);
                cursesRender(mainFrame, hintMsg);
                break;

            default: break;
        }
        return GameState::NotEnd;
    }
}

void msgSender(int sockFd, const Sig sig, const Action action) {
    sendChar(sockFd, static_cast<char>(sig));
    sendChar(sockFd, static_cast<char>(action));
}