#include"serverUtils.h"

int xMin, yMin, xMax, yMax, playerNum, waitTimeMs, semsetId;

void setTicker(int waitTimeMs) {
    itimerval timeset;
    timeset.it_interval.tv_sec = waitTimeMs / 1000;
    timeset.it_interval.tv_usec = 1000 * (waitTimeMs % 1000);
    timeset.it_value.tv_sec = 0;
    timeset.it_value.tv_usec = (waitTimeMs == 0 ? 0 : 1);
    // if waitTimeMs is 0, then stop the ticker, 
    // otherwise start ticker in a usec
    
    setitimer(ITIMER_REAL, &timeset, nullptr);
}

std::pair<int, int> locGen() {
    std::pair<int, int> pr{0, 0};
    pr.first = (xMin + 1) + rand() % (xMax - (xMin + 1));
    pr.second = yMin + rand() % (yMax - yMin);
    return pr;
}

void cleanUp(int flag) {
    semctl(semsetId, 0, IPC_RMID, nullptr);
    exit(0);
}