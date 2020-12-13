/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "lloydManager.h"

//GLOBAL
int finish = 0;
int terminate = 0;
semaphore sem_dataReady;
semaphore sem_dataProcessed;

//FUNCTIONS
static void* handleTextFile(void* args){
    StationStatistics* stations = (StationStatistics *) args;
    while (!terminate){
        sleep(REWRITE_TIME);
        print(LLOYD_FILE_REWRITE);
        writeToFile(stations);
    }
    return (void*) 0;
}

void ksighandler(){
    finish = 1;
    terminate = 1;

    exit(EXIT_FAILURE);
}

int main(){

    StationStatistics* stations;
    StationData* shared;

    pthread_t tid;

    //Reprogram signals
    signal(SIGINT, ksighandler);

    //Init shared and dynamic memory
    shared = initMemory(&stations);
    print(STARTING);

    //Create thread to manage the Hallorann.txt rewriting
    if (pthread_create(&tid, NULL, handleTextFile, &stations) != 0) print(ERROR_THREAD);

    //Communicate with Jack
    while (!finish) {
        jackCommunication(shared, stations);
    }


    print(DISCONNECTING);
    //TODO: Wait for all threads to join
    pthread_join(tid, NULL);
    //TODO: Free all dynamic data
    freeSharedMemory(shared);
    int i;
    for(i = 0; i < (int) sizeof(stations); i++){
        free(stations[i].nameString);
    }
    free(stations);

    return 0;
}
