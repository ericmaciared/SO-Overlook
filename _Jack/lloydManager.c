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

//PRIVATE FUNCTIONS
void showStationStatistics(StationStatistics* station){
    print(station->nameString);
    print(EOL);
    printf("%f\n", station->temperature);
    print(EOL);
    printf("%f\n", station->humidity);
    print(EOL);
    printf("%f\n", station->pressure);
    print(EOL);
    printf("%f\n", station->precipitation);
    print(EOL);
}

void freeStationStatistics(StationStatistics* data){
    free(data->nameString);
}

//PUBLIC FUNCTIONS

//TODO
int jackCommunication(StationDataShared* shared, StationStatistics* stations, semaphore* sem_dataReady, semaphore* sem_dataProcessed, int* numStations){
    // Wait for Jack signal to start reading
    print(SEM_WAITING);
    SEM_wait(sem_dataReady);

    // Read data and process it
    print(RECEIVING_DATA);
    readFromMemory(shared, stations, numStations);

    // Signal Jack about process done
    print(PROCESSED_DATA);
    SEM_signal(sem_dataProcessed);

    return 0;
}

int readFromMemory(StationDataShared* shared, StationStatistics* stations, int* numStations){
    char buffer [64];
    int i = 0;
    print("Checking stations size");
    print(EOL);
    if(strlen(shared->nameString) <= 0){
        return -1;
    }
    if(sizeof(stations) > sizeof(StationStatistics*)){
        while (i < (int) sizeof(stations)){
            if (strcmp(stations[i].nameString, shared->nameString) == 0){
                stations[i].temperature = (stations[i].temperature * stations[i].readings + shared->temperature) / (stations[i].readings + 1);
                stations[i].humidity = (stations[i].humidity * stations[i].readings + shared->humidity) / (stations[i].readings + 1);
                stations[i].temperature = (stations[i].pressure * stations[i].readings + shared->pressure) / (stations[i].readings + 1);
                stations[i].temperature = (stations[i].precipitation * stations[i].readings + shared->precipitation) / (stations[i].readings + 1);
                stations[i].readings++;
                return 0;
            }
            i++;;
        }
    }
    sprintf(buffer, "%d numStations, %ld \n\n", *numStations, sizeof(StationStatistics));
    write(1, buffer, sizeof(buffer));
    stations = realloc(stations, (*numStations + 1) * sizeof(StationStatistics*));
    strcpy(stations[i].nameString, shared->nameString);
    stations[i].temperature = shared->temperature;
    stations[i].humidity = shared->humidity;
    stations[i].pressure = shared->pressure;
    stations[i].precipitation = shared->precipitation;
    stations[i].readings = 1;
    *numStations = *numStations+1;
    return 1;
}

StationDataShared* initMemory(){
    int memid;
    StationDataShared *sharedStation;

    memid = shmget(IPC_PRIVATE,sizeof(StationData),IPC_CREAT|IPC_EXCL|0600);
    if (memid>0){
        sharedStation = shmat(memid,0,0);
        return sharedStation;
    } else{
        exit(EXIT_FAILURE);
    }
}

void writeToFile(StationStatistics* statistics, const char* file){
    int fd = -1;
    char buffer[512];

    fd = open(file, O_WRONLY | O_CREAT);

    if (fd <= 0) {
        perror(ERROR_FILE);
    }
    else {
        if (statistics != NULL){
            int i = 0;
            for (i = 0; i < (int) sizeof(statistics) ; i++){
                sprintf(buffer, STATION_STATISTICS, statistics[i].nameString, statistics[i].temperature, statistics[i].humidity, statistics[i].pressure, statistics[i].precipitation);
                buffer[strlen(buffer)+1] = '\0';
                write(fd, buffer, sizeof(buffer));
            }
        }
    }
}

void freeSharedMemory(StationDataShared* shared){
    free(shared);
}

static void* handleTextFile(){
    //signal(SIGINT, ksighandler);
    while (!terminate){
        sleep(REWRITE_TIME);
        print(LLOYD_FILE_REWRITE);
        writeToFile(stations, FILE_PATH);
    }
    return (void*) 0;
}

void* lloyd(){

    StationStatistics* stations;
    int numStations = 1;
    StationDataShared* shared;

    SEM_constructor_with_name(&sem_dataReady, ftok("jack.c", 'a'));
    SEM_constructor_with_name(&sem_dataProcessed, ftok("jack.c", 'b'));

    pthread_t tid;

    //Reprogram signals
    //signal(SIGINT, ksighandler);

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
