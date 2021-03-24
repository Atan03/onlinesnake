#include"commonUtils.h"

void setPoll(pollfd *pollArr, int fd, int events, int &pollArrIdx) {
    pollArr[pollArrIdx].fd = fd;
    pollArr[pollArrIdx].events = events; 
    pollArr[pollArrIdx].revents = 0;
    ++pollArrIdx;
}

bool valid2Args(char **argv) {
    return true;
    //implement later
}

bool valid3Args(char **argv) {
    return true;
    //implement later
}