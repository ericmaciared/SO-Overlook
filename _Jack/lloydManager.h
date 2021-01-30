/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

#ifndef _LLOYDMANAGER_H_
#define _LLOYDMANAGER_H_

//LIBRARIES

//MODULES
#include "libraries.h"
#include "functions.h"
#include "jackManager.h"
#include "protocolManager.h"

//DEFINES
#define EOL "\n"
#define ERROR_CONFIG "Error opening configuration file.\n\n"
#define ERROR_ARGS "Error processing arguments.\n\n"
#define ERROR_SOCKET "Error processing socket\n\n"
#define ERROR_BIND "Error binding socket\n\n"
#define ERROR_ACCEPT "Error accepting new connection\n\n"
#define ERROR_THREAD "Error creating new thread\n\n"
#define ERROR_SHARED_MEMORY "Error creating shared memory\n\n"
#define ERROR_FILE "Error opening file"

#define LLOYD_PROMPT "$Lloyd:\n"
#define SEM_WAITING "Waiting for semaphore...\n"
#define RECEIVING_DATA "Receiving data...\n"
#define PROCESSED_DATA "Data processed correctly!\n"
#define LLOYD_FILE_REWRITE "\n$Lloyd:\nRewriting Hallorann.txt\n"

#define HALLORANN_PATH "Hallorann.txt"
#define REWRITE_TIME 300

typedef struct StationStatistics{
    char nameString[100];
    int readings;
    float temperature;
    float humidity;
    float pressure;
    float precipitation;
} StationStatistics;

typedef struct LloydStruct{
    StationStatistics* stations;
    int numStations;
} LloydStruct;

/**
 * This function will read station data in the shared memory
 * and process it into the station data array. It will also
 * compute for the data averages of the stations
 * @param: station pointer to struct with name and socket fd
 * @return: char value for new response from server
 */
int readFromMemory(StationDataShared* shared, LloydStruct* lloyd_struct);

/**
 * This function will write the statistics to the Hallorann.txt
 * @param: station pointer to array of data to write
 * @return: int value for error control
 */
void writeToFile(StationStatistics* stations, const char* file, int numStations);

/**
 * This function will print the statistics of a given station
 * @param: station pointer to struct of data to print
 * @return: print
 */
 void showStationStatistics(StationStatistics* station);
#endif
