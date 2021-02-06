/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "jackManager.h"
#include "lloydManager.h"
#include "protocolManager.h"

//GLOBAL
semaphore sem_dataReady;
semaphore sem_dataProcessed;
semaphore sem_statistics;

int finish = 0;
int terminate = 0;

StationDataShared* shared;
int memidShared;

LloydStruct lloyd_struct;


//FUNCTIONS
void ksighandler(){
    finish = 1;
    terminate = 1;
    signal(SIGINT, ksighandler);
}

void handleTextFile(){
    //SEM_wait(&sem_statistics);
    writeToFile(lloyd_struct.stations, HALLORANN_PATH, lloyd_struct.numStations);
    //SEM_signal(&sem_statistics);
}

void ksigalarmhandler(){
    handleTextFile();
    signal(SIGALRM, ksigalarmhandler);
    alarm(REWRITE_TIME);
}

int lloyd(){

    signal(SIGALRM, ksigalarmhandler);
    alarm(REWRITE_TIME);

    lloyd_struct.stations = (StationStatistics*) malloc(sizeof(StationStatistics));
    lloyd_struct.numStations = 0;

    //Communicate with Jack
    while (!finish) {
        SEM_wait(&sem_dataReady);
        if (errno == EINTR) continue;

        // Make sure we are not printing
        //SEM_wait(&sem_statistics);
        readFromMemory(shared, &lloyd_struct);
        //SEM_signal(&sem_statistics);

        // Signal Danny about process done
        SEM_signal(&sem_dataProcessed);
    }

    free(lloyd_struct.stations);
    return 0;
}

char readFromDanny(Station* client){
    StationData sd;
    StationDataShared sds;
    char type = protocolRead(client->sockfd, &sd);

    switch(type){
        // Receiving actual Data
        case 'D':
            print(PROMPT);
            print(RECEIVING_DATA);
            sds = convertToStationShared(&sd, client->name);
            showStationData(&sd);

            SEM_wait(&sem_dataProcessed);
            writeToSharedMemory(shared, &sds);
            SEM_signal(&sem_dataReady);

            freeStationData(&sd);
            break;
        // Erroneous frame
        case 'K':
            print("\n$Jack:\nErroneous Frame\n");
            //sendtodanny('K')
            break;
        // None of the previous, can include X and Q
        default:
            break;
    }
    return type;
}

static void* handleDanny(void* args){
    Station* client = (Station *) args;
    char buffer[64];
    char type = 0;
    struct pollfd pfd;

    pfd.fd = client->sockfd;
    pfd.events = POLLIN;

    // Check loop to read from Danny
    while (!terminate){
        if (poll(&pfd, 1, 0) >= 0 || terminate){
            if (pfd.revents & POLLIN){
                type = readFromDanny(client);
                print("Received type = ");
                write(1, &type, 1);
                if (type == 'Q' || type == 'X') break;
                else replyToDanny(client, type);
            }
        }
    }

    sprintf(buffer, "\nClosing %s station.\n", client->name);
    print(buffer);

    //Close client socket
    close(client->sockfd);

    return (void *) client;
}

int main(int argc, char const *argv[]){
    Config config;
    int sockfd;
    Station clients[32];
    pthread_t tid[32];
    int i = 0;
    struct pollfd pfd;
    pid_t lloydPID;

    // Reprogram signals

    // Check correct arguments
    if (argc <= 1 || argc > 2){
        print(ERROR_ARGS);
        exit(EXIT_FAILURE);
    }

    // Fill config file
    if(!processConfig(&config, argv[argc -1])) exit(EXIT_FAILURE);
    print(STARTING);

    // Create socket
    sockfd = initServer(&config);
    if (sockfd < 0) {
        free(config.ip);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    // Accept connections and assign threads indefinitely
    pfd.fd = sockfd;
    pfd.events = POLLIN;

    // Init shared memory
    if((memidShared = shmget(IPC_PRIVATE,sizeof(StationDataShared),IPC_CREAT|IPC_EXCL|0600)) < 0) {
        free(config.ip);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if((shared = (StationDataShared*) shmat(memidShared,NULL,0)) == (StationDataShared*) - 1) {
        free(config.ip);
        close(sockfd);
        shmctl(memidShared, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }

    // Semaphores init and creation
    SEM_constructor(&sem_dataReady);
    SEM_constructor(&sem_dataProcessed);
    SEM_constructor(&sem_statistics);

    SEM_init(&sem_dataReady, 0);
    SEM_init(&sem_dataProcessed, 1);
    SEM_init(&sem_statistics, 1);

    // Create fork for Lloyd
    if ((lloydPID = fork()) < 0) {
        free(config.ip);
        close(sockfd);
        shmdt(shared);
        shmctl(memidShared, IPC_RMID, NULL);
        SEM_destructor(&sem_dataReady);
        SEM_destructor(&sem_dataProcessed);
        SEM_destructor(&sem_statistics);
        exit(EXIT_FAILURE);
    }

    if (lloydPID == 0){
        //Lloyd process
        signal(SIGINT, ksighandler);
        lloyd();
        shmdt(shared);
    }
    else{
        signal(SIGINT, ksighandler);
        print(PROMPT);
        print(CONNECTION_WAITING);
        while (!finish) {
            if (poll(&pfd, 1, -1) >= 0 || finish) {
                if (pfd.revents & POLLIN){
                    if (acceptConnection(sockfd, &clients[i]) < 0 || finish) break;
                    if (pthread_create(&tid[i], NULL, handleDanny, &clients[i]) != 0){
                        print(ERROR_THREAD);
                        close(clients[i].sockfd);
                        i--;
                    }
                    else i++;
                }
            }
        }

        print(DISCONNECTING);

        // Wait for lloyd fork to finish
        wait(NULL);

        // Wait for all danny threads to join
        for (int j = 0; j < i; j++){
            pthread_join(tid[j], NULL);
            if(clients[j].name != NULL) free(clients[j].name);
        }

        // Free all dynamic data
        free(config.ip);

        close(sockfd);

        shmdt(shared);
        shmctl(memidShared, IPC_RMID, NULL);

        SEM_destructor(&sem_dataReady);
        SEM_destructor(&sem_dataProcessed);
        SEM_destructor(&sem_statistics);
    }

    return 0;
}
