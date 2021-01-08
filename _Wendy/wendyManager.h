/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

#ifndef _WENDYMANAGER_H_
#define _WENDYMANAGER_H_

//LIBRARIES

//MODULES
#include "libraries.h"
#include "functions.h"
#include "protocolManager.h"


//DEFINES
#define EOL "\n"
#define ERROR_CONFIG "Error opening configuration file.\n\n"
#define ERROR_ARGS "Error processing arguments.\n\n"
#define ERROR_SOCKET "Error processing socket\n\n"
#define ERROR_BIND "Error binding socket\n\n"
#define ERROR_ACCEPT "Error accepting new connection\n\n"
#define ERROR_THREAD "Error creating new thread\n\n"

#define PROMPT "\n$Wendy:\n"
#define STARTING "\nStarting Wendy...\n"
#define DISCONNECTING "\nDisconnecting Wendy...\n\n"
#define BYEDANNY "Disconnecting Danny (%s)\n\n"
#define CONNECTION_WAITING "Waiting...\n"
#define NEW_CONNECTION "New connection: %s\n"
#define RECEIVING_DATA "Receiving data...\n"


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
 *         pointer to string to fill with received data
 * @return: char value for new response from server
 */
char readFromDanny(Station* client, char* out);

/**
 * This function will reply the Danny client with the message
 * corresponding to the responseType.
 * @param: station pointer to struct with name and socket fd
 *         char value that contains the response type to reply
 * @return: int value set to 1 if need to terminate, else 0
 */
int replyToDanny(Station* client, char responseType);

int dataToImageData(char* in, char* imageName, char* md5sum);

int checkMD5SUM(char* origin, char* current);


#endif
