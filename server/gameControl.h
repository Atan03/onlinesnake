#ifndef _GAMECONTROLL_H_
#define _GAMECONTROLL_H_

#include"serverUtils.h"
#include"classFile/Dot.h"
#include"classFile/Snake.h"
#include"../common/socketLib.h"

extern std::unordered_set<std::shared_ptr<Dot>> dotPtrSet;
extern std::unordered_map<int, std::shared_ptr<Snake>> playerList;
extern std::string hintStr;
extern int pipeInPrnt, sockFdOutPrnt, pipeInGame, pipeOutGame,
    pipeInSig, pipeOutSig, pipeInTask, pipeOutTask, counterBkwd;

/* Signal handler function of SIGALRM, 
   send something to pipe for uniformlly process */
void sigHandle(int flag);

/* Generate a uncollide location pair based on given game states */
std::pair<int, int> unCollideLocGen();

/* Generate a legal shared_ptr of Dot */
std::shared_ptr<Dot> dotPtrGen();

/* Generate a legal shared_ptr of Snake, 
   given snakeCnt to decide which symbol to use */
std::shared_ptr<Snake> snakePtrGen(int snakeCnt);

/* Generate playerList and dotPtrSet according to game configuration,
   clear hintStr and set backward counter, setup signal and ticker*/
void gameInit();

/* Send current game frame to every player(alive) */
void sendFrame();

/* Send information according to requirements and argument */
void sendInfo(InfoType infoType, int arg);

/* Set hintStr according to requirement and playerSymbol */
void setHint(HintType hintType, char playerSymbol = 'O');

/* Return whether the game terminates */
bool gameTerminal();

/* Check game frame and kill targets being eaten, 
   then cut hungry snakes' tails */
void targetKiller();

/* Check game frame and register snakes being collided, 
   then tell poll to refresh and call playerKilled to kill snakes */
void snakeKiller();

/* Check targets needed, then generate corresponding targets */
void targetRecover();

/* Fetch signal from pipe and then check whether in bakcward counting.
 * If counting, send information and when counting is over set ticker.
 * If not, do stepForward for every player and do game process, 
 * then check whether the game is over and end up the game
 */
void gameStep();

/* Snakes got something to do*/
void snakeReact(int sockFd);

/* Put new direction from clients to action list */
void snakeReSteer(int sockFd, Action action);

/* Player choose to quit, close its socket and erase player from platerList*/
void playerQuit(int sockFd);
/* Player died, close its socket and erase player from platerList*/
void playerKilled(int sockFd);

/* add player to playerList, and start game when ready */
void addPlayer(int sockFd);

#endif