/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "lloydManager.h"

#define FILE_PATH ".Hallorann.txt"

//GLOBAL
int finish = 0;
int terminate = 0;
semaphore sem_dataReady;
semaphore sem_dataProcessed;
StationStatistics* stations;

//FUNCTIONS
void ksighandler(){
    finish = 1;
    terminate = 1;
}

static void* handleTextFile(){
    signal(SIGINT, ksighandler);
    while (!terminate){
        sleep(REWRITE_TIME);
        print(LLOYD_FILE_REWRITE);
        writeToFile(stations, FILE_PATH);
    }
    return (void*) 0;
}

int main(){

    StationStatistics* stations;
    int numStations = 1;
    StationData* shared;

    SEM_constructor_with_name(&sem_dataReady, ftok("jack.c", 'a'));
    SEM_constructor_with_name(&sem_dataProcessed, ftok("jack.c", 'b'));

    pthread_t tid;

    //Reprogram signals
    signal(SIGINT, ksighandler);

    //Init shared and dynamic memory
    stations = (StationStatistics*) malloc(sizeof(StationStatistics));
    shared = initMemory();
    print(STARTING);

    //Create thread to manage the Hallorann.txt rewriting
    if (pthread_create(&tid, NULL, handleTextFile, NULL) != 0) print(ERROR_THREAD);

    //Communicate with Jack
    while (!finish) {
        print("!finish");
        jackCommunication(shared, stations, &sem_dataReady, &sem_dataProcessed, &numStations);
    }

    print(DISCONNECTING);
    //TODO: Wait for all threads to join
    pthread_join(tid, NULL);

    print("Thread joined");
    //TODO: Free all dynamic data
    freeSharedMemory(shared);
    int i;
    for(i = 0; i < (int) sizeof(stations); i++){
        free(stations[i].nameString);
    }
    free(stations);

    return 0;
}
