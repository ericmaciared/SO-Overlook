/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/


//MODULES
#include "libraries.h"
#include "functions.h"
#include "lloydManager.h"

#define FILE_PATH ".Hallorann.txt"

//PRIVATE FUNCTIONS
void showStationStatistics(StationStatistics* station){
    char buffer[255];
    sprintf(buffer, STATION_STATISTICS, station->nameString, station->temperature, station->humidity, station->pressure, station->precipitation);
    print(buffer);
}

//PUBLIC FUNCTIONS
int readFromMemory(StationDataShared* shared, StationStatistics* stations, int *numStations){
    //char buffer [64];
    int i = 0;
    if(strlen(shared->nameString) <= 0){
        print("\n$Lloyd:\nREADMEM - Station name missing\n");
        return -1;
    }
    while (i < *numStations){
        if (strcmp(stations[i].nameString, shared->nameString) == 0){
            stations[i].temperature = (stations[i].temperature * stations[i].readings + shared->temperature) / (stations[i].readings + 1);
            stations[i].humidity = (stations[i].humidity * stations[i].readings + shared->humidity) / (stations[i].readings + 1);
            stations[i].pressure = (stations[i].pressure * stations[i].readings + shared->pressure) / (stations[i].readings + 1);
            stations[i].precipitation = (stations[i].precipitation * stations[i].readings + shared->precipitation) / (stations[i].readings + 1);
            stations[i].readings++;
            return 0;
        }
        i++;;
    }

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

void writeToFile(StationStatistics* statistics, const char* file, int numStations){
    int fd = -1;
    char buffer[512];
    int i;
    fd = open(file, O_WRONLY | O_CREAT);

    if (fd <= 0) perror(ERROR_FILE);
    else {
        if (statistics != NULL){
            print(LLOYD_FILE_REWRITE);

            for (i = 0; i < numStations; i++){
                sprintf(buffer, STATION_STATISTICS, statistics[i].nameString, statistics[i].temperature, statistics[i].humidity, statistics[i].pressure, statistics[i].precipitation);
                //print(buffer);
                buffer[strlen(buffer)+1] = '\0';
                if (write(fd, buffer, strlen(buffer)) < 0) print("Error in write");
            }
        }
        close(fd);
    }
}



