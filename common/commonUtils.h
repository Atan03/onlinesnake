#ifndef _COMMONUTILS_H_
#define _COMMONUTILS_H_

#include<poll.h>

/* Indicate which direction a snake should move forward to */
enum class Action {kHold, kUp, kDown, kLeft, kRight};

/* Indicate which kind of message is being transfered through internet */
enum class Sig {Frame, GameOver, Info, Hello, Control, Setting, Hi};

/* Indicate game state from server to client */
enum class GameState {Win, Lose, NotEnd};

/* Setup poll events according to the arguments */
void setPoll(pollfd *pollArr, int fd, int events, int &pollArrIdx); 

/* To test whether argv[1] and argv[2] is valid */
bool valid2Args(char **argv);

/* To test whether argv[1] and argv[2] is valid */
bool valid3Args(char **argv);

#endif