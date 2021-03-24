#include"gameProcess.h"

void mainProcess(int pipeIn, int sockFdOut) {
    // Set global variables: pipes, sockets
    pipeInPrnt = pipeIn;    sockFdOutPrnt = sockFdOut;
    int pipeFds[2];
    pipe(pipeFds);
    pipeInGame = pipeFds[1];    pipeOutGame = pipeFds[0];
    pipe(pipeFds);
    pipeInSig = pipeFds[1];     pipeOutSig = pipeFds[0];
    pipe(pipeFds);
    pipeInTask = pipeFds[1];    pipeOutTask = pipeFds[0];
    while(true) {
        // Set game configuration: screen size, timetick
        // Reserved for setting during a communication process with client
        playerNum = 3;
        waitTimeMs = 200;
        xMin = 0;    yMin = 0;
        xMax = 20;    yMax = 60;

        char c;
        while(true) {
            int pollArrIdx = 0, 
                tmpPlayerNum = static_cast<int>(playerList.size());
            
            // Setting poll events
            pollfd pollArr[tmpPlayerNum + 3];
            for(auto &pr : playerList) {
                setPoll(pollArr, pr.first, POLLIN | POLLRDHUP, pollArrIdx);
            }
            setPoll(pollArr, pipeOutGame, POLLIN, pollArrIdx);
            setPoll(pollArr, sockFdOutPrnt, POLLIN, pollArrIdx);
            setPoll(pollArr, pipeOutSig, POLLIN, pollArrIdx);

            int pollState = poll(pollArr, tmpPlayerNum + 3, -1);
            if(pollState == -1) {
                if(errno == 4) {
                    // Poll being interupted by signal(SIGALRM), restart
                    continue;
                }
                throw std::runtime_error("Poll failure. errno: " + std::to_string(errno));
            }

            if(pollArr[tmpPlayerNum].revents & POLLIN) {
                int readLen = read(pipeOutGame, &c, sizeof(c));
                if(readLen == -1) {
                    throw std::runtime_error("Read failure. errno: " + std::to_string(errno));
                }

                if(c == static_cast<char>(Operation::Refresh)) {
                    // Game processor ask for refresh poll, player killed or quited
                    continue;
                }
                else if(c == static_cast<char>(Operation::GameEnd)){
                    // Game processor ask for end up, game ended
                    break;
                }
            }
            if(pollArr[tmpPlayerNum + 1].revents & POLLIN) {
                // Unix socket transfered a new socket's fd and context, receiving...
                int sockFd = recvFd(sockFdOut);
                if(sockFd == -1) {
                    throw std::runtime_error("Read failure. errno: " + std::to_string(errno));
                }
                struct sembuf wakeSem[1] {0, 1, SEM_UNDO};
                int semState = semop(semsetId, wakeSem, 1);
                if(semState == -1) {
                    throw std::runtime_error("Sem operate failure. errno: " + std::to_string(errno));
                }

                addPlayer(sockFd);
            }
            if(pollArr[tmpPlayerNum + 2].revents & POLLIN) {
                // Unification of SIGALRM, now step the game forward
                gameStep();
            }
            for(int i = 0 ; i < tmpPlayerNum ; ++i) {
                if(pollArr[i].revents & POLLRDHUP) {
                    // Got some player quit
                    playerQuit(pollArr[i].fd);
                    break;
                }
                else if(pollArr[i].revents & POLLIN) {
                    // Got some player's input
                    snakeReact(pollArr[i].fd);
                }
            }
        }
    }
}