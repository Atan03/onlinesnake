#ifndef _CURSESLIB_H_
#define _CURSESLIB_H_

#include<curses.h>
#include<climits>
#include"clientUtils.h"

enum class ClearScr{Clear, DontClear};

/* Setup curses configures and do initial works */
void setUpCurses();

/* Close related jobs */
void endCurses();

/* Draw symbols with corresponding locations to curses cache according to mainFrame */
void myCursesFrame(const MainFrame &mainFrame);

/* Draw hingMsg with its contained information to curses cache */
void myCursesHint(HintMsg &hintMsg);

/* Draw a line of string from the location arguments on */
void myCursesStr(const std::string &str, int locX, int locY, 
    ClearScr clearScr = ClearScr::Clear);

/* Draw a line of string from the default location: (0, 0) on */
void myCursesStr(const std::string &str, ClearScr clearScr = ClearScr::Clear);

/* Clear the cache, draw mainFrame and hintMsg on curses's cache and render */
void cursesRender(const MainFrame &mainFrame, HintMsg &hintMsg);

#endif