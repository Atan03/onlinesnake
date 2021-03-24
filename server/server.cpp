#include"gameProcess.h"
#include"fcntl.h"

using namespace std;

/* Set fd to be non-blocking with fcntl */
void setNonBlocking(int fd) {
    int oldOption = fcntl(fd, F_GETFL);
    int newOption = oldOption | O_NONBLOCK;
    fcntl(fd, F_SETFL, newOption);
}

/* Distribute sockFd to process according to its config, reserved to implement */
void processDstri(int sockFd, const pair<int, int> &ioPipeSock) {
    static bool availableProcess = true;
    char c = static_cast<char>(Operation::Default);
    int readLen = 0;
    while((readLen = read(ioPipeSock.first, &c, 1)) > 0);
    if(readLen == -1 && errno != 11) {
        throw runtime_error("Read from pipe failure. errno: " + to_string(errno));
    }

    Operation opr = static_cast<Operation>(c);
    if(opr == Operation::NotAvailable) {
        availableProcess = false;
    }
    else if(opr == Operation::Available) {
        availableProcess = true;
    }

    if(availableProcess) {
        sendFd(ioPipeSock.second, sockFd);
        
        struct sembuf waitSem[1] {0, -1, SEM_UNDO};
        int semState = semop(semsetId, waitSem, 1);
        if(semState == -1) {
            throw runtime_error("Sem operate failure. errno: " + to_string(errno));
        }
    }
    close(sockFd);
}

/* Create a pool of processes, reserved to implement */
pair<int, int> createProcessPool() {
    int pipeFdsFromP[2], sockPairToP[2];
    int pipeState = pipe(pipeFdsFromP);
    if(pipeState == -1) {
        throw runtime_error("Pipe create failure. errno: " + to_string(errno));
    }
    int sockState = socketpair(AF_UNIX, SOCK_STREAM, 0, sockPairToP);
    if(sockState == -1) {
        throw runtime_error("UNIX sock create failure. errno: " + to_string(errno));
    }  

    semsetId = semget(SEMKEY, 1, (0666 | IPC_CREAT | IPC_EXCL));
    if(semsetId == -1) {
        throw runtime_error("Semaphore create failure. errno: " + to_string(errno));
    }
    union semun initVal;
    initVal.val = 0;
    int semState = semctl(semsetId, 0, SETVAL, initVal);    
    if(semState == -1) {
        throw runtime_error("Semaphore contrl failure. errno: " + to_string(errno));
    }
    
    int pidRet = fork();
    if(pidRet == 0) {
        mainProcess(pipeFdsFromP[1], sockPairToP[0]);
    }

    setNonBlocking(pipeFdsFromP[0]);
    return {pipeFdsFromP[0], sockPairToP[1]};
}

int main(int argc, char **argv) {
    int sockFd = 0;
    if(argc == 2 && valid2Args(argv)) {
        sockFd = makeServerSocket(argv[1], DFLPORT, DFLBACKLOG);
    }
    else if(argc == 3 && valid3Args(argv)) {
        sockFd = makeServerSocket(argv[1], stoi(argv[2]), DFLBACKLOG);
    } 
    else {
        cout << "Format: [local ip] [(optional)listened port]" << endl;
        return 1;
    }
    
    signal(SIGCHLD, SIG_IGN);
    // Handle zombie process
    signal(SIGINT, cleanUp);
    // Handle semaphore

    pair<int, int> ioPipeSock = createProcessPool();

    while(true) {
        int acceptedFd = acceptErrTrw(sockFd);
        if(acceptedFd == -1) {
            throw runtime_error("Accept failure. errno: " + to_string(errno));
        }
        processDstri(acceptedFd, ioPipeSock);
    }

    return 0;
}