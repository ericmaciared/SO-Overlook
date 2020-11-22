/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

#ifndef _JACKMANAGER_H_
#define _JACKMANAGER_H_

//LIBRARIES
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
//#include <conio.h>


//MODULES
#include "functions.h"


//DEFINES
#define EOL "\n"
#define ERROR_CONFIG "Error opening configuration file.\n\n"
#define ERROR_ARGS "Error processing arguments.\n\n"
#define ERROR_SOCKET "Error processing socket\n\n"
#define ERROR_BIND "Error binding socket\n\n"
#define ERROR_ACCEPT "Error accepting new connection\n\n"

#define STARTING "\nStarting Jack...\n\n"
#define DISCONNECTING "\nDisconnecting Jack...\n\n"
#define CONNECTION_WAITING "Waiting for connection...\n"


//STRUCTS
typedef struct{
    char* ip;
    int port;
}Config;

typedef struct StationData{
  char* dateString;
  char* hourString;
  char* temperatureString;
  char* humidityString;
  char* pressureString;
  char* precipitationString;
}StationData;


//FUNCTIONS
/*
* This function will process the configuration file
* @param: config as a pointer to the config struct to fill
*         file as an address to a file to read from
* @return: 1 if successful 0 if failure
*/
int processConfig(Config* config, const char* file);


/*
* This will create the socket and configure it to take
* new connections
* @param: config pointer to configuration info struct
* @return: fd of the new connection socket
*/
int initServer(Config* config);


int acceptConnection(int sockfdServer);

void readFromClient(int sockfd, StationData* station);


#endif
