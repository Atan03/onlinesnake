#include"../common/socketLib.h"
#include"cursesLib.h"
#include"clientUtils.h"
#include"msgLib.h"

using namespace std;

void gameProcess(int sockFd, MainFrame &mainFrame, HintMsg &hintMsg) {
    GameState gameState;
    char c;

    hintMsg = HintMsg(EMPTYSTR, INT_MAX);

    pollfd pollArr[2];
    int idx = 0;
    setPoll(pollArr, 0, POLLIN, idx);
    setPoll(pollArr, sockFd, POLLIN | POLLRDHUP, idx);

    while(true) {
        int pollState = poll(pollArr, 2, -1);
        if(pollState == -1) {
            throw runtime_error("Poll failure.");
        }

        if(pollArr[1].revents & POLLRDHUP) {
            return;
        }
        else if(pollArr[1].revents & POLLIN) {
            if((gameState = msgRecvr(sockFd, mainFrame, hintMsg)) != GameState::NotEnd) {
                setMainFrameFromGameState(mainFrame, gameState);
                hintMsg = HintMsg(ANYKEYHINT, INT_MAX);
                cursesRender(mainFrame, hintMsg);
                
                close(sockFd);
                c = getchar();
                return;
            }
        }
        
        if(pollArr[0].revents & POLLIN) {
            c = getchar();
            if(isControllChar(c)) {
                msgSender(sockFd, Sig::Control, actionTsfr(c));
            }
            else if(c == 'q') {
                frameFromStr(mainFrame, EMPTYSTR);
                hintMsg = HintMsg(USERQUIT, INT_MAX);
                cursesRender(mainFrame, hintMsg);
                
                close(sockFd);
                getchar();
                return;
            }
            else {
                continue;
            }
        }
    }
}

bool waitProcess(int sockFd, const MainFrame &mainFrame, HintMsg &hintMsg) {
    char c;
    
    pollfd pollArr[2];
    int idx = 0;
    setPoll(pollArr, 0, POLLIN, idx);
    setPoll(pollArr, sockFd, POLLIN | POLLRDHUP, idx);
    
    while(true) {
        int pollState = poll(pollArr, 2, -1);
        if(pollState == -1) {
            throw runtime_error("Poll failure.");
        }

        if(pollArr[1].revents & POLLRDHUP) {
            return false;
        }
        else if(pollArr[1].revents & POLLIN) {
            return true;
        }
        
        if(pollArr[0].revents & POLLIN) {
            c = getchar();
            if(c == 'q') {
                hintMsg = HintMsg(USERQUIT, INT_MAX);
                cursesRender(mainFrame, hintMsg);
                
                close(sockFd);
                getchar();
                return false;
            }
            else {
                continue;
            }
        }
    }
    return false;
}

int main(int argc, char **argv) {
    if(!(argc == 2 || argc == 3) || 
        (argc == 2 && !valid2Args(argv)) ||
        (argc == 2 && !valid3Args(argv))) {
        
        cout << "Format: [local ip] [(optional)listened port]" << endl;
        return 1;
    }

    // Reserved for communicating with server to set game configuration
    xMin = 0, yMin = 0, xMax = 20, yMax = 60, playerNum = 3, waitTimeMs = 200;
    setUpCurses();

    while(true) {
        char c;
        HintMsg hintMsg;
        MainFrame mainFrame;

        frameFromStr(mainFrame, WELCOME);
        hintMsg = HintMsg(WELCOMEHINT, INT_MAX);
        cursesRender(mainFrame, hintMsg);
        do {
            c = getchar();
            if(c == 'q') {
                endCurses();
                return 0;
            }
            else if(c == 's'){
                break;
            }
        } while (true);

        frameFromStr(mainFrame, EMPTYSTR);
        hintMsg = HintMsg(CONNECTING, INT_MAX);
        cursesRender(mainFrame, hintMsg);
        int sockFd;
        if(argc == 2) {
            sockFd = connectToServer(argv[1], DFLPORT);
        }
        else {
            sockFd = connectToServer(argv[1], stoi(argv[2]));
        }

        hintMsg = HintMsg(CONNECTED, INT_MAX);
        cursesRender(mainFrame, hintMsg);
        if(!waitProcess(sockFd, mainFrame, hintMsg)) {
            continue;
        }

        gameProcess(sockFd, mainFrame, hintMsg);
    }

    endCurses();
    return 0;
}