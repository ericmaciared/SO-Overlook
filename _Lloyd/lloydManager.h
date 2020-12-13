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


//DEFINES
#define EOL "\n"
#define ERROR_CONFIG "Error opening configuration file.\n\n"
#define ERROR_ARGS "Error processing arguments.\n\n"
#define ERROR_SOCKET "Error processing socket\n\n"
#define ERROR_BIND "Error binding socket\n\n"
#define ERROR_ACCEPT "Error accepting new connection\n\n"
#define ERROR_THREAD "Error creating new thread\n\n"
#define ERROR_SHARED_MEMORY "Error creating shared memory\n\n"

#define LLOYD_PROMPT "$Lloyd:\n"
#define STARTING "\nStarting Lloyd...\n\n"
#define DISCONNECTING "\nDisconnecting Lloyd...\n\n"
#define SEM_WAITING "Waiting...\n"
#define RECEIVING_DATA "Receiving data...\n"
#define PROCESSED_DATA "Data processed correctly!\n"
#define LLOYD_FILE_REWRITE "Rewriting Hallorann.txt\n"

#define REWRITE_TIME 120

typedef struct StationData{
    char* nameString;
    float temperature;
    float humidity;
    float pressure;
    float precipitation;
} StationData;

typedef struct StationStatistics{
    char* nameString;
    int readings;
    float temperature;
    float humidity;
    float pressure;
    float precipitation;
} StationStatistics;


//FUNCTIONS
/**
 * This will create the shared memory address and
 * initialize the stations data array
 * @param: stations statistics pointer to be initialized
 * @return: pointer of the shared memory address
 */
StationData* initMemory(StationStatistics** stations);

/**
 * This function will be in charge of managing the communication
 * with Jack, waiting for signals, reading and sending them back
 * @param:
 * @return: int value for new client socket for error control
 */
int jackCommunication(StationData* shared, StationStatistics* stations);

/**
 * This function will read station data in the shared memory
 * and process it into the station data array. It will also
 * compute for the data averages of the stations
 * @param: station pointer to struct with name and socket fd
 * @return: char value for new response from server
 */
int readFromMemory(StationData* shared, StationStatistics** stations);

/**
 * This function will write the statistics to the Hallorann.txt
 * @param: station pointer to array of data to write
 * @return: int value for error control
 */
void writeToFile(StationStatistics* stations);

/**
 * This function will free the shared memory with Jack
 * @param: station pointer to shared memory
 * @return: int value for error control
 */
void freeSharedMemory(StationData* shared);

#endif
