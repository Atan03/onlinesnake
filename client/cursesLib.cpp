#include"cursesLib.h"

void setUpCurses() {
    initscr();
    noecho();
    crmode();
    clear();
}

void endCurses() {
    endwin();
}

void myCursesFrame(const MainFrame &mainFrame) {
    for(auto &pixel : mainFrame) {
        mvaddch(pixel.xLoc, pixel.yLoc, pixel.symbol);
    }
}

void myCursesHint(HintMsg &hintMsg) {
    if(hintMsg.msgDuration != INT_MAX && hintMsg.msgDuration != 0){
        --hintMsg.msgDuration;
    }
    if(hintMsg.msgDuration == 0) {
        hintMsg = HintMsg(EMPTYSTR, INT_MAX);
    }

    myCursesStr(hintMsg.msg, ClearScr::DontClear);
}

void myCursesStr(const std::string &str, int locX, int locY, ClearScr clearScr) {
    if(clearScr == ClearScr::Clear) {
        clear();
    }
    move(locX, locY);
    addstr(str.c_str());
    refresh();
}

void myCursesStr(const std::string &str, ClearScr clearScr) {
    myCursesStr(str, xMin, yMin, clearScr);
}

void cursesRender(const MainFrame &mainFrame, HintMsg &hintMsg) {
    clear();
    myCursesFrame(mainFrame);
    myCursesHint(hintMsg);
    refresh();
}