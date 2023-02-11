#include "gameControl.h"

std::unordered_set<std::shared_ptr<Dot>> dotPtrSet;
std::unordered_map<int, std::shared_ptr<Snake>> playerList;
std::string hintStr;
int pipeInPrnt, sockFdOutPrnt, pipeInGame, pipeOutGame,
    pipeInSig, pipeOutSig, pipeInTask, pipeOutTask, counterBkwd;

void sigHandle(int flag) {
    char c = static_cast<char>(Operation::SignalReady);
    int writeLen = write(pipeInSig, &c, sizeof(c));
    if(writeLen == -1) {
        throw std::runtime_error("Write to pipe failure. errno: " + 
            std::to_string(errno));
    }
}

std::pair<int, int> unCollideLocGen() {
    while(true) {
        auto locPr = locGen();
        bool availableLoc = true;
        for(auto &pr : playerList) {
            if(pr.second != nullptr && pr.second->collideLoc(locPr)) {
                availableLoc = false;
                break;
            }
        }
        if(availableLoc) {
            for(auto &dotPtr : dotPtrSet) {
                if(dotPtr->getLoc() == locPr) {
                    availableLoc = false;
                    break;
                }
            }
        }

        if(availableLoc) {
            return locPr;
        }
    }
}

std::shared_ptr<Dot> dotPtrGen() {
    return std::make_shared<Dot>(unCollideLocGen(), '*');
}

std::shared_ptr<Snake> snakePtrGen(int snakeCnt) {
    return std::make_shared<Snake>(unCollideLocGen(), AvailableSymbol[snakeCnt]);
}

void gameInit() {
    int snakeCnt = 0;
    for(auto &pr : playerList) {
        pr.second = snakePtrGen(snakeCnt++);
    }
    for(size_t i = 0 ; i < playerList.size() ; ++i) {
        dotPtrSet.insert(dotPtrGen());
    }

    hintStr.clear();
    counterBkwd = COUNTERBKWDINIT;
    signal(SIGALRM, sigHandle);
    setTicker(WAITTIMEBKWDMS);
}

void sendFrame() {
    myProto::FrameMsg frameMsg;
    for(auto &pr : playerList) {
        pr.second->protoAdd(frameMsg);
    }
    for(auto &dot : dotPtrSet) {
        dot->protoAdd(frameMsg);
    }
    if(!hintStr.empty()) {
        frameMsg.set_hint_msg(hintStr);
    }

    for(auto &pr : playerList) {
        sendChar(pr.first, static_cast<char>(Sig::Frame));
        sendProtoMsg(pr.first, frameMsg);
    }
}

void sendInfo(InfoType infoType, int arg) {
    myProto::StrMsg strMsg;
    switch(infoType) {
        case InfoType::GameStart:
            strMsg.set_main_msg(GAMESTART + std::to_string(arg));
            for(const auto &pr : playerList) {
                if(arg == 3) {
                    setHint(HintType::Init, pr.second->getSymbol());
                    strMsg.set_hint_msg(hintStr);
                }
                else {
                    hintStr.clear();
                }
                
                sendChar(pr.first, static_cast<char>(Sig::Info));
                sendProtoMsg(pr.first, strMsg);
            }
            break;

        default:
            break;
    }
}

void setHint(HintType hintType, char playerSymbol) {
    std::string snakeExample = 
        playerSymbol + std::string(4, tolower(playerSymbol));
    switch(hintType) {
        case HintType::Init:
            hintStr = "Your snake is represented as: " + snakeExample;
            break;
        
        case HintType::Killed:
            hintStr += "Player " + snakeExample + " was killed. ";
            break;

        case HintType::Quit:
            hintStr += "Player " + snakeExample + " quited. ";
            break;

        default:
            break;
    }
}

bool gameTerminal() {
    bool gameTerminates = playerList.size() <= 1;
    return gameTerminates;
}

void targetKiller() {
    std::unordered_set<std::shared_ptr<Dot>> killingDots;
    std::unordered_set<std::shared_ptr<Snake>> growingSnakes;
    for(auto &dotPtr : dotPtrSet) {
        for(auto &pr : playerList) {
            if(*(pr.second) == *dotPtr) {
                killingDots.insert(dotPtr);
                growingSnakes.insert(pr.second);
            }
        }
    }

    for(auto &dotPtr : killingDots) {
        dotPtrSet.erase(dotPtr);
    }
    for(auto &pr : playerList) {
        if(growingSnakes.find(pr.second) == growingSnakes.end()) {
            pr.second->tailCut();
        }
    }
}

void snakeKiller() {
    std::unordered_set<int> killingPlayers;
    for(auto &prSnakeMain : playerList) {
        for(auto &prSnakeVice : playerList) {
            if(prSnakeMain.second->collideLoc(prSnakeVice.second->getLoc())) {
                killingPlayers.insert(prSnakeVice.first);
            }

            if(prSnakeMain.first != prSnakeVice.first && 
                (*(prSnakeMain.second) == *(prSnakeVice.second))) {
                killingPlayers.insert(prSnakeMain.first);
                killingPlayers.insert(prSnakeVice.first);
            }
        }
    }

    if(!killingPlayers.empty()) {
        char c = static_cast<char>(Operation::Refresh);
        write(pipeInGame, &c, 1);
    }
    for(auto &sockFd : killingPlayers) {
        sendChar(sockFd, static_cast<char>(Sig::GameOver));
        sendChar(sockFd, static_cast<char>(GameState::Lose));
        playerKilled(sockFd);
    }
}

void targetRecover() {
    int targetNeed = static_cast<int>(playerList.size()) - 
        static_cast<int>(dotPtrSet.size());
    while(targetNeed > 0) {
        dotPtrSet.insert(dotPtrGen());
        --targetNeed;
    }
}

void gameStep() {
    char c;
    int readLen = read(pipeOutSig, &c, 1);
    if(readLen == -1) {
        throw std::runtime_error("Read from pipe failure. errno: " + 
            std::to_string(errno));
    }

    //if doing backward counting, execute the following instructions and return
    if(counterBkwd != 0) {
        --counterBkwd;
        sendInfo(InfoType::GameStart, counterBkwd);
        if(counterBkwd == 0) {
            setTicker(waitTimeMs);
        }
        return;
    }

    for(auto &pr : playerList) {
        pr.second->stepForward(xMin, xMax, yMin, yMax);
    }
    
    targetKiller();
    snakeKiller();
    targetRecover();

    //if game terminates, execute the following instructions
    if(gameTerminal()) {
        if(!playerList.empty()) {
            sendChar(playerList.begin()->first, static_cast<char>(Sig::GameOver));
            sendChar(playerList.begin()->first, static_cast<char>(GameState::Win));
        }

        setTicker(0);
        signal(SIGALRM, SIG_DFL);
        for(auto &pr : playerList) {
            close(pr.first);
        }
        dotPtrSet.clear();
        playerList.clear();

        char c = static_cast<char>(Operation::GameEnd);
        write(pipeInGame, &c, 1);
        c = static_cast<char>(Operation::Available);
        write(pipeInPrnt, &c, 1);
    }
    else {
        sendFrame();
        hintStr.clear();
    }
}

void snakeReact(int sockFd) {
    //Reserved for more complex control
    char c = recvChar(sockFd);
    if(static_cast<Sig>(c) == Sig::Control) {
        snakeReSteer(sockFd, static_cast<Action>(recvChar(sockFd)));
    }
}

void snakeReSteer(int sockFd, Action action) {
    (playerList)[sockFd]->setActionToList(action);
}

void playerQuit(int sockFd) {
    close(sockFd);
    if(playerList[sockFd] != nullptr) {
        setHint(HintType::Quit, (playerList)[sockFd]->getSymbol());
    }
    playerList.erase(sockFd);
}

void playerKilled(int sockFd) {
    close(sockFd);
    setHint(HintType::Killed, (playerList)[sockFd]->getSymbol());
    playerList.erase(sockFd);
}

void addPlayer(int sockFd) {
    playerList.insert({sockFd, nullptr});    

    //if game starts, execute the following instructions
    if(static_cast<int>(playerList.size()) == playerNum) {
        char c = static_cast<char>(Operation::NotAvailable);
        int writeLen = write(pipeInPrnt, &c, sizeof(c));
        if(writeLen == -1) {
            throw std::runtime_error("Write to pipe failure. errno: " + 
                std::to_string(errno));
        }

        gameInit();
    }
}