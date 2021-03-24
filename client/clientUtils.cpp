#include"clientUtils.h"

int xMin, yMin, xMax, yMax, playerNum, waitTimeMs;

int getMidxLocStart() {
    return ((xMin + 1) + xMax) / 2;
}

int getMidyLocStart(const std::string &str) {
    return (yMin + yMax - str.size()) / 2;
}

bool isControllChar(char c) {
    return c == 'i' || c == 'j' || c == 'k' || c == 'l';
}

void frameFromStr(MainFrame &mainFrame, const std::string &str) {
    mainFrame.clear();
    
    int xLoc = getMidxLocStart(), yLoc = getMidyLocStart(str);
    for(auto &c : str) {
        mainFrame.emplace_back(xLoc, yLoc++, c);
    }
}

void setMainFrameFromGameState
    (MainFrame &mainFrame, GameState gameState) {
    switch(gameState) {
        case GameState::Win:
            frameFromStr(mainFrame, WIN);
            break;
            
        case GameState::Lose:
            frameFromStr(mainFrame, LOSE);
            break;

        default:
            break;
    }
}

Action actionTsfr(char c) {
    switch(c) {
        case 'i': return Action::kUp;
        case 'k': return Action::kDown;
        case 'j': return Action::kLeft;
        case 'l': return Action::kRight;
        default : return Action::kHold;
    }
}