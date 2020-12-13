/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//INCLUDES
#include "lloydManager.h"

//PRIVATE FUNCTIONS
void showStationData(StationData* station){
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

void freeStationData(StationData* data){
    free(data->nameString);
}

//PUBLIC FUNCTIONS

//TODO
int jackCommunication(StationData* shared, StationStatistics* stations){

    // Wait for Jack signal to start reading
    print(SEM_WAITING);
    wait(&sem_dataReady);

    // Read data and process it
    print(RECEIVING_DATA);
    readFromMemory(shared, &stations);

    // Signal Jack about process done
    print(PROCESSED_DATA);
    signal(&sem_dataProcessed);

    return 0;
}

int readFromMemory(StationData* shared, StationStatistics** stations){
    int i = 0;
    while (i < (int) sizeof(stations)){
        if (strcmp(stations[i]->nameString, shared->nameString) == 0){
            stations[i]->temperature = (stations[i]->temperature * stations[i]->readings + shared->temperature) / (stations[i]->readings + 1);
            stations[i]->humidity = (stations[i]->humidity * stations[i]->readings + shared->humidity) / (stations[i]->readings + 1);
            stations[i]->temperature = (stations[i]->pressure * stations[i]->readings + shared->pressure) / (stations[i]->readings + 1);
            stations[i]->temperature = (stations[i]->precipitation * stations[i]->readings + shared->precipitation) / (stations[i]->readings + 1);
            stations[i]->readings++;
            return 0;
        }
        i++;
    }
    stations = realloc(stations, (sizeof(StationStatistics)+1) * sizeof(StationStatistics));
    strcpy(stations[i]->nameString, shared->nameString);
    stations[i]->temperature = shared->temperature;
    stations[i]->humidity = shared->humidity;
    stations[i]->pressure = shared->pressure;
    stations[i]->precipitation = shared->precipitation;
    stations[i]->readings = 1;
    return 1;
}

StationData* initMemory(StationStatistics** stations){
    int memid;
    StationData *sharedStation;

    stations = (StationStatistics**) malloc(sizeof(StationStatistics*));

    memid = shmget(IPC_PRIVATE,sizeof(StationData),IPC_CREAT|IPC_EXCL|0600);
    if (memid>0){
        sharedStation = shmat(memid,0,0);
        return sharedStation;
    } else{
        exit(EXIT_FAILURE);
    }
}

void writeToFile(StationStatistics* statistics){
    free(statistics);
}

void freeSharedMemory(StationData* shared){
    free(shared);
}
