#ifndef _GAMEPROCESS_H_
#define _GAMEPROCESS_H_

#include"gameControl.h"

/* Process game IO and call for work thread and work function */
void mainProcess(int pipeIn, int pipeOut);

#endif