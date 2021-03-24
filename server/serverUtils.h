#ifndef _SERVERUTILS_H_
#define _SERVERUTILS_H_

#include<vector>
#include<unordered_map>
#include<initializer_list>
#include<sys/time.h>
#include<sys/sem.h>
#include"../common/commonUtils.h"

/* Indicate what kind of hint should be sent to client */
enum class HintType {Init, Quit, Killed};

/* Indicate what kind of info should be sent to client*/
enum class InfoType {GameStart};
// reserved for more expandable function

/* Indicate transfered operation during game process */
enum class Operation {Default, NotAvailable, Available, SignalReady, Refresh, GameEnd};

/* Represent four directs with pair */
struct Step {
    int xStep, yStep;

    Step(int putXStep, int putYStep): xStep(putXStep), yStep(putYStep) {}
    Step(const std::initializer_list<int> il): 
        xStep(*il.begin()), yStep(*(il.end() - 1)) {}
};

/* Defined to use in semCtl */
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *arr;
};

/* Used config/globals in server */
extern int xMin, yMin, xMax, yMax, playerNum, waitTimeMs, semsetId;

const int WAITTIMEBKWDMS = 1000, COUNTERBKWDINIT = 4, SEMKEY = 99;

const std::string GAMESTART("Get ready! Game starting in ");

/* Available symbol sequence to represent a snake */
const std::vector<char> AvailableSymbol
    {'O', 'X', 'H', 'C', 'E', 'K'};

/* Map from a enum class Action to a step */
const std::unordered_map<Action, Step> stepMap
    {{Action::kUp, {-1, 0}}, 
     {Action::kDown, {1, 0}},
     {Action::kLeft, {0, -1}}, 
     {Action::kRight, {0, 1}}};

/* Set ticker tick SIGALRM every waitTimeMs, starting immediately,
   if waitTimeMs is 0, disable ticker */
void setTicker(int waitTimeMs);

/* Generate random location pair matchin <(xMin, xMax], (yMin, yMax]> */
std::pair<int, int> locGen();

/* Clean out semaphore */
void cleanUp(int flag);

#endif