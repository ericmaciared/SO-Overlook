/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

#ifndef _JACKMANAGER_H_
#define _JACKMANAGER_H_

//LIBRARIES

//MODULES
#include "libraries.h"
#include "functions.h"
#include "protocolManager.h"
#include "semaphore_v2.h"

//DEFINES
#define EOL "\n"
#define ERROR_CONFIG "Error opening configuration file.\n\n"
#define ERROR_ARGS "Error processing arguments.\n\n"
#define ERROR_SOCKET "Error processing socket\n\n"
#define ERROR_BIND "Error binding socket\n\n"
#define ERROR_ACCEPT "Error accepting new connection\n\n"
#define ERROR_THREAD "Error creating new thread\n\n"

#define PROMPT "\n$Jack:\n"
#define STARTING "\nStarting Jack...\n"
#define DISCONNECTING "\nDisconnecting Jack...\n\n"
#define DISCONNECTING_LLOYD "\nDisconnecting Lloyd...\n\n"
#define BYEDANNY "Disconnecting Danny (%s)\n\n"
#define CONNECTION_WAITING "Waiting...\n"
#define NEW_CONNECTION "New connection: %s\n"
#define RECEIVING_DATA "Receiving data...\n"
#define STATION_STATISTICS "%s\n\tTemperature: %.2f\n\tHumidity: %.2f%%\n\tPressure: %.2f\n\tPrecipitation: %.2f\n\n"


//STRUCTS
typedef struct{
    char* ip;
    int port;
}Config;

typedef struct{
  char* name;
  int sockfd;
} Station;

//FUNCTIONS
/**
 * This function will process the configuration file
 * @param: config as a pointer to the config struct to fill
 *         file as an address to a file to read from
 * @return: 1 if successful 0 if failure
 */
int processConfig(Config* config, const char* file);

/**
 * This will create the socket and configure it to take
 * new connections
 * @param: config pointer to configuration info struct
 * @return: fd of the new connection socket for server
 */
int initServer(Config* config);

/**
 * This function will wait for incomming connections and will
 * accept them, returning socket connection for client.
 * @param: int value for the server socket
 * @return: int value for new client socket
 *          -1 if unsuccessful connection
 */
int acceptConnection(int sockfdServer, Station* client);

/**
 * This function will read incoming messages of size 115,
 * process the data read and return a value corresponding
 * to the response type that is required to reply with.
 * @param: station pointer to struct with name and socket fd
 * @return: char value for new response from server
 */
char readFromDanny(Station* client);

/**
 * This function will reply the Danny client with the message
 * corresponding to the responseType.
 * @param: station pointer to struct with name and socket fd
 *         char value that contains the response type to reply
 * @return: int value set to 1 if need to terminate, else 0
 */
int replyToDanny(Station* client, char responseType);

/**
 * This function will convert to output a StationDataShared
 * @param: StationData pointer to data struct
 *         char pointer with station name
 * @return: StationDataShared filled with the converted information
 */
StationDataShared convertToStationShared(StationData* station, char* name);

/**
 * This function will print the data of a station
 * @param: StationData* to data
 * @return: print
 */
void showStationData(StationData* station);

/**
 * This function will print the data of a station shared
 * @param: StationData* to data
 * @return: print
 */
void showStationSharedData(StationDataShared* station);

/**
 * This function will free the data of a station and its shared conversion
 * @param: StationData* to data
 *         StationDataShared* to converted data
 * @return:
 */
void freeStationData(StationData* data);

/**
 * This function will convert to output a StationDataShared
 * @param: StationData pointer to data struct
 *         char pointer with station name
 * @return: StationDataShared filled with the converted information
 */
void writeToSharedMemory(StationDataShared* shared, StationDataShared* new);

/**
 * This function will free the shared memory with Jack
 * @param: station pointer to shared memory
 * @return: int value for error control
 */
void freeSharedMemory(StationDataShared* shared);

#endif
