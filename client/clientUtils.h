#ifndef _SERVERUTILS_H_
#define _SERVERUTILS_H_

#include<string>
#include<vector>
#include"../common/commonUtils.h"
#include"../common/socketLib.h"
#include"../protoFile/protoMsg.pb.h"

struct HintMsg {
    std::string msg;
    int msgDuration;

    HintMsg() = default;
    HintMsg(const std::string &putMsg, int putFrameDuration):
        msg(putMsg), msgDuration(putFrameDuration) {}
};

struct Pixel {
    int xLoc, yLoc;
    char symbol;

    Pixel() = default;
    Pixel(int putXLoc, int putYLoc, char putSymbol):
        xLoc(putXLoc), yLoc(putYLoc), symbol(putSymbol) {}
};

typedef std::vector<Pixel> MainFrame;

extern int xMin, yMin, xMax, yMax, playerNum, waitTimeMs;

const int DFLHINTDUR = 5;

const std::string EMPTYSTR(""),
    WELCOME("GreedySnake!"),
    WELCOMEHINT("Press \'s\' to start, \'q\' to quit."),
    CONNECTING("Connecting..."),
    CONNECTED("Connected, waiting for other players. Press \'q\' to quit."),
    USERQUIT("Userquit. Press any key..."),
    WIN("YOU WON!!!"),
    LOSE("Game Over..."),
    ANYKEYHINT("Press any key...");

int getMidxLocStart();

int getMidyLocStart(const std::string &str);

bool isControllChar(char c);

void frameFromStr(MainFrame &mainFrame, const std::string &str);

void setMainFrameFromGameState
    (MainFrame &mainFrame, GameState gameState);

Action actionTsfr(char c);

#endif 