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
int readFromMemory(StationDataShared* shared, LloydStruct* lloyd_struct){
    int i = 0;
    if(strlen(shared->nameString) <= 0){
        print("\n$Lloyd:\nREADMEM - Station name missing\n");
        return -1;
    }

    while (i < lloyd_struct->numStations){
        if (strcmp(lloyd_struct->stations[i].nameString, shared->nameString) == 0){
            lloyd_struct->stations[i].temperature = (lloyd_struct->stations[i].temperature * 
            lloyd_struct->stations[i].readings + shared->temperature) / 
            (lloyd_struct->stations[i].readings + 1);

            lloyd_struct->stations[i].humidity = (lloyd_struct->stations[i].humidity *
            lloyd_struct->stations[i].readings + shared->humidity) / 
            (lloyd_struct->stations[i].readings + 1);

            lloyd_struct->stations[i].pressure = (lloyd_struct->stations[i].pressure * 
            lloyd_struct->stations[i].readings + shared->pressure) / 
            (lloyd_struct->stations[i].readings + 1);

            lloyd_struct->stations[i].precipitation = (lloyd_struct->stations[i].precipitation *
            lloyd_struct->stations[i].readings + shared->precipitation) / 
            (lloyd_struct->stations[i].readings + 1);

            lloyd_struct->stations[i].readings++;
            return 0;
        }
        i++;
    }

    //New station
    lloyd_struct->stations = realloc(lloyd_struct->stations, 
        (lloyd_struct->numStations + 1) * sizeof(StationStatistics));
    strcpy(lloyd_struct->stations[i].nameString, shared->nameString);
    lloyd_struct->stations[i].temperature = shared->temperature;
    lloyd_struct->stations[i].humidity = shared->humidity;
    lloyd_struct->stations[i].pressure = shared->pressure;
    lloyd_struct->stations[i].precipitation = shared->precipitation;
    lloyd_struct->stations[i].readings = 1;
    lloyd_struct->numStations += 1;

    return 0;
}

void writeToFile(StationStatistics* statistics, const char* file, int numStations){
    int fd = -1;
    char buffer[512];
    int i;
    fd = open(file, O_WRONLY | O_CREAT, 0777);

    if (fd <= 0) perror(ERROR_FILE);
    else {
        if (statistics != NULL){
            print(LLOYD_FILE_REWRITE);

            for (i = 0; i < numStations; i++){
                sprintf(buffer, STATION_STATISTICS, statistics[i].nameString, 
                statistics[i].temperature, statistics[i].humidity, 
                statistics[i].pressure, statistics[i].precipitation);

                buffer[strlen(buffer)+1] = '\0';
                if (write(fd, buffer, strlen(buffer)) < 0) print("Error in write");
            }
        }
        close(fd);
    }
}




