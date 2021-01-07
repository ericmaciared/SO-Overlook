/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

#ifndef _DANNYMANAGER_H_
#define _DANNYMANAGER_H_

//LIBRARIES

//MODULES
#include "libraries.h"
#include "functions.h"
#include "protocolManager.h"


//DEFINES
#define EOL "\n"
#define TESTING "Testing...\n"
#define FILES_FOUND "%d files found\n"
#define NO_FILES "No files available\n"
#define ERROR_CONFIG "Error opening configuration file.\n\n"
#define ERROR_ARGS "Error processing arguments.\n\n"
#define ERROR_SOCKET "Error processing socket\n\n"
#define ERROR_IP "Error processing invalid IP\n\n"
#define ERROR_CONNECT "Error connecting to server\n\n"

#define PROMPT "\n$%s\n"
#define STARTING "\nStarting Danny...\n\n"
#define DISCONNECT_DANNY "Disconnecting Danny...\n\n"
#define CONNECTING_JACK "Connecting Jack...\n"
#define CONNECTED_JACK "Connected to Jack.\n"
#define DISCONNECT_JACK "\nDisconnecting Jack...\n"
#define CONNECTING_WENDY "Connecting Wendy...\n"
#define CONNECTED_WENDY "Connected to Wendy.\n"
#define DISCONNECT_WENDY "\nDisconnecting Wendy...\n"

#define SENDING_DATA "Sending data...\n"
#define DATA_SENT "Data sent.\n"

#define NOFILE 0
#define TXT 1
#define JPG 2


//STRUCTS
typedef struct{
    char* ip;
    int port;
} Config;

typedef struct Data{
  char* station;
  char* path;
  int time;
  Config jack;
  Config wendy;
} Data;

typedef struct{
  char* name;
  int jacksockfd;
  int wendysockfd;
} Station;


//FUNCTIONS
/**
 * Processes the data from a config.txt file and store it
 * @param: data points to the Data struct to fill
 *         file indicates file address to read from
 * @return: 1 if successful 0 if error
 */
int processConfig(Data* data, const char* file);

/**
 * Frees all the stored dynamic config data
 * @param: data pointer to structure
 */
void freeConfig(Data* data);

/**
 * Creates and binds socket for server communication with
 * a Server process.
 * @param: data pointer to configuration info structure
 *        station struct to information on station
 *        id of server (1 for Jack, 0 for Wendy)
 * @return: sockfd if successful, -1 if error
 */

int connectToServer(Data* data, Station station, int serverID);

/**
 * Scans for new files in stored data directory and handles
 * sending and processing of information
 * @param: data pointer to structure with config information
 *          station to structure with both server sockets and name
 */
int scanDirectory(Data* data, Station station);

/**
 * Sends Jack a Disconnection message and leaves
 * @param: station struct to information on station
 */
void disconnectJack(Station* station);

/**
 * Sends Wendy a Disconnection message and leaves
 * @param: station struct to information on station
 */
void disconnectWendy(Station* station);



#endif
