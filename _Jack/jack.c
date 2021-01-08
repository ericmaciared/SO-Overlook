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
int volatile finish = 0;
int volatile terminate = 0;
int volatile writing = 0;
int volatile printing = 0;
StationDataShared* shared;
int memidShared;


//FUNCTIONS
void ksighandler(){
    //finish = 1;
    terminate = 1;

    signal(SIGINT, ksighandler);
}

char readFromDanny(Station* client){
    StationData sd;
    StationDataShared sds;
    char type = protocolRead(client->sockfd, &sd);

    switch(type){
        case 'D':
            print(JACK_PROMPT);
            print(RECEIVING_DATA);
            sds = convertToStationShared(&sd, client->name);

            SEM_wait(&sem_dataProcessed);
            //print("Writing to Shared\n\n");
            writeToSharedMemory(shared, &sds);
            SEM_signal(&sem_dataReady);

            //showStationSharedData(shared);
            freeStationData(&sd);
            break;
        case 'K':
            //Erroneous frame
            print("Erroneous Frame\n");

            //sendtodanny('K')
            break;
        default:
            //sendtodanny('Z')
            break;
    }

    return type;
}


static void* handleTextFile(void* args){
    LloydStruct* ls = (LloydStruct*) args;
    //StationStatistics* stations = ls->stations;
    //int numStations = ls.numStations;
    //char buffer[64];

    signal(SIGINT, ksighandler);
    while (!finish){
        sleep(REWRITE_TIME);
        while(writing && !finish){
        }
        printing = 1;
        print(LLOYD_FILE_REWRITE);
        /*
        sprintf(buffer, "Address numStations = %p\n", ls->numStations);
        print(buffer);
        sprintf(buffer, "Address numStations = %p\n", numStations);
        print(buffer);
        sprintf(buffer, "numStations = %d\n", *ls->numStations);
        print(buffer);
        sprintf(buffer, "numStations = %d\n", *numStations);
        print(buffer);
        */
        writeToFile(&ls->stations, HALLORANN_PATH, ls->numStations);

        printing = 0;
    }
    return (void*) 0;
}

static void* handleDanny(void* args){
    Station* client = (Station *) args;
    char buffer[64];
    char type = 0;
    struct pollfd pfd;

    pfd.fd = client->sockfd;
    pfd.events = POLLIN;

    while (!terminate){
        while (!terminate){
            if (poll(&pfd, 1, 0) >= 0){
                if (pfd.revents & POLLIN){
                    type = readFromDanny(client);
                    break;
                }
            }
        }
        if (type == 'Q' || type == 'X') break;
        else replyToDanny(client, type);

    }

    sprintf(buffer, "\nClosing %s station.\n", client->name);
    print(buffer);

    //Close client socket
    close(client->sockfd);

    return (void *) client;
}


int lloyd(){
    //char buffer[64];
    //StationStatistics* stations;
    //int *numStations;
    pthread_t printtid;
    LloydStruct lloyd_struct;
    //int i;
    char buffer[64];

    lloyd_struct.stations = (StationStatistics*) malloc(sizeof(StationStatistics));
    //lloyd_struct->numStations = (int*) malloc(sizeof(int));

    //Create thread to manage the Hallorann.txt rewriting
    //stations= lloyd_struct->stations;
    //numStations = lloyd_struct->numStations;

    sprintf(buffer, "Address numStations = %p\n", &lloyd_struct.numStations);
    print(buffer);

    if (pthread_create(&printtid, NULL, handleTextFile, &lloyd_struct) != 0) print(ERROR_THREAD);

    //Communicate with Jack
    while (!finish) {
        // Wait for Jack signal to start reading
        print(SEM_WAITING);
        SEM_wait(&sem_dataReady);

        // Make sure we are not printing
        while (printing){
            print("Lloyd: PRINTING\n");
            sleep(1);
        }
        writing = 1;
        readFromMemory(shared, lloyd_struct.stations, &lloyd_struct.numStations);
        writing = 0;

        // Signal Jack about process done
        SEM_signal(&sem_dataProcessed);
    }


    pthread_join(printtid, NULL);
    print("Print Thread joined\n");

    free(lloyd_struct.stations);
    //free(numStations);
    return 0;
}


int main(int argc, char const *argv[]){
    Config config;
    int sockfd;
    Station clients[32];
    pthread_t tid[32];
    int i = 1;
    struct pollfd pfd;
    pid_t lloydPID;
    //char buffer[64];

    //Reprogram signals
    signal(SIGINT, ksighandler);

    //Create and Init Semaphores
    SEM_constructor(&sem_dataReady);
    SEM_init(&sem_dataReady, 0);

    SEM_constructor(&sem_dataProcessed);
    SEM_init(&sem_dataProcessed, 1);

    //Check correct arguments
    if (argc <= 1 || argc > 2){
        print(ERROR_ARGS);
        exit(EXIT_FAILURE);
    }

    //Fill config file
    if(!processConfig(&config, argv[argc -1])) exit(EXIT_FAILURE);
    print(STARTING);

    //Init shared and dynamic memory
    if ((memidShared = shmget(IPC_PRIVATE,sizeof(StationDataShared),IPC_CREAT|IPC_EXCL|0600)) < 0) exit(EXIT_FAILURE);
    if((shared = (StationDataShared*) shmat(memidShared,NULL,0)) == (StationDataShared*) - 1) exit(EXIT_FAILURE);

    //Semaphores
    SEM_constructor(&sem_dataReady);
    SEM_constructor(&sem_dataProcessed);
    SEM_init(&sem_dataReady, 0);
    SEM_init(&sem_dataProcessed, 1);

    //Create FORK for Lloyd
    if ((lloydPID = fork()) < 0){
        exit(EXIT_FAILURE);

    } if (lloydPID == 0){
        lloyd();
        shmdt(shared);
    } else{
        //Create socket
        sockfd = initServer(&config);
        if (sockfd < 0) {
            exit(EXIT_FAILURE);
        }
        //Accept connections and assign threads indefinitely
        pfd.fd = sockfd;
        pfd.events = POLLIN;

        while (!terminate) {
            if (poll(&pfd, 1, 0) >= 0) {
                if (pfd.revents & POLLIN){
                    if (acceptConnection(sockfd, &clients[i]) < 0 || terminate) break;
                    if (pthread_create(&tid[i], NULL, handleDanny, &clients[i]) != 0){
                        print(ERROR_THREAD);
                        close(clients[i].sockfd);
                        i--;
                        break;
                    }
                    else{
                        i++;
                    }

                }
            }
        }

        print(DISCONNECTING);

        //TODO: Wait for all threads to join
        for (int j = 0; j < i; j++){
            pthread_join(tid[j], NULL);
        }
        print("Threads Joined\n");

        //TODO: Free all dynamic data
        free(config.ip);
        for (int j = 0; j < i; j++){
            free(clients[j].name);
        }
        shmdt(shared);
        shmctl(memidShared, IPC_RMID, NULL);

        SEM_destructor(&sem_dataReady);
        SEM_destructor(&sem_dataProcessed);
    }

    SEM_destructor(&sem_dataReady);
    SEM_destructor(&sem_dataProcessed);

    return 0;
}
