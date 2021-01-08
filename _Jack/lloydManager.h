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
<<<<<<< Updated upstream:_Jack/lloydManager.h
=======
#include "protocolManager.h"
>>>>>>> Stashed changes:_Lloyd/lloydManager.h

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
<<<<<<< Updated upstream:_Jack/lloydManager.h
#define SEM_WAITING "Waiting...\n"
=======
#define SEM_WAITING "Waiting for semaphore...\n"
>>>>>>> Stashed changes:_Lloyd/lloydManager.h
#define RECEIVING_DATA "Receiving data...\n"
#define PROCESSED_DATA "Data processed correctly!\n"
#define LLOYD_FILE_REWRITE "Rewriting Hallorann.txt\n"

<<<<<<< Updated upstream:_Jack/lloydManager.h

#define REWRITE_TIME 20
=======
#define HALLORANN_PATH "Hallorann.txt"

#define REWRITE_TIME 10
>>>>>>> Stashed changes:_Lloyd/lloydManager.h

typedef struct StationStatistics{
    char nameString[STATION_NAME_LENGTH];
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

<<<<<<< Updated upstream:_Jack/lloydManager.h
//FUNCTIONS
/**
 * This will create the shared memory address and
 * initialize the stations data array
 * @param:
 * @return: pointer of the shared memory address
 */
StationDataShared* initMemory();

/**
 * This function will be in charge of managing the communication
 * with Jack, waiting for signals, reading and sending them back
 * @param:
 * @return: int value for new client socket for error control
 */
int jackCommunication(StationDataShared* shared, StationStatistics* stations, semaphore* sem_dataReady, semaphore* sem_dataProcessed, int* numStations);
=======

>>>>>>> Stashed changes:_Lloyd/lloydManager.h

/**
 * This function will read station data in the shared memory
 * and process it into the station data array. It will also
 * compute for the data averages of the stations
 * @param: station pointer to struct with name and socket fd
 * @return: char value for new response from server
 */
int readFromMemory(StationDataShared* shared, StationStatistics* stations, int* numStations);

/**
 * This function will write the statistics to the Hallorann.txt
 * @param: station pointer to array of data to write
 * @return: int value for error control
 */
void writeToFile(StationStatistics** stations, const char* file, int numStations);

/**
 * This function will print the statistics of a given station
 * @param: station pointer to struct of data to print
 * @return: print
 */
<<<<<<< Updated upstream:_Jack/lloydManager.h
void freeSharedMemory(StationDataShared* shared);

/**
 * This function will execute the Lloyd Procedure
 * @param:
 * @return:
 */
void* lloyd();
=======
void showStationStatistics(StationStatistics* station);

>>>>>>> Stashed changes:_Lloyd/lloydManager.h

#endif
