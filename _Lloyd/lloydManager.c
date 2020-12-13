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
int jackCommunication(StationData* shared, StationStatistics* stations, semaphore* sem_dataReady, semaphore* sem_dataProcessed, int* numStations){
    // Wait for Jack signal to start reading
    print(SEM_WAITING);
    SEM_wait(sem_dataReady);

    // Read data and process it
    print(RECEIVING_DATA);
    readFromMemory(shared, &stations, numStations);

    // Signal Jack about process done
    print(PROCESSED_DATA);
    SEM_signal(sem_dataProcessed);

    return 0;
}

int readFromMemory(StationData* shared, StationStatistics** stations, int* numStations){
    char buffer [64];
    int i = 0;
    print("Checking stations size");
    print(EOL);
    if(sizeof(stations) > sizeof(StationStatistics*)){
        while (i < (int) sizeof(stations)){
            if (strcmp(stations[i]->nameString, shared->nameString) == 0){
                stations[i]->temperature = (stations[i]->temperature * stations[i]->readings + shared->temperature) / (stations[i]->readings + 1);
                stations[i]->humidity = (stations[i]->humidity * stations[i]->readings + shared->humidity) / (stations[i]->readings + 1);
                stations[i]->temperature = (stations[i]->pressure * stations[i]->readings + shared->pressure) / (stations[i]->readings + 1);
                stations[i]->temperature = (stations[i]->precipitation * stations[i]->readings + shared->precipitation) / (stations[i]->readings + 1);
                stations[i]->readings++;
                return 0;
            }
            i++;;
        }
    }
    sprintf(buffer, "%d numStations, %ld \n\n", *numStations, sizeof(StationStatistics));
    write(1, buffer, sizeof(buffer));
    stations = realloc(stations, (*numStations + 1) * sizeof(StationStatistics*));
    strcpy(stations[i]->nameString, shared->nameString);
    stations[i]->temperature = shared->temperature;
    stations[i]->humidity = shared->humidity;
    stations[i]->pressure = shared->pressure;
    stations[i]->precipitation = shared->precipitation;
    stations[i]->readings = 1;
    *numStations = *numStations+1;
    return 1;
}

StationData* initMemory(){
    int memid;
    StationData *sharedStation;

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

void freeSharedMemory(StationData* shared){
    free(shared);
}
