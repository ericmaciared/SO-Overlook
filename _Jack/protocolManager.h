/**
 * Authors: Eric Macià Redondo (eric.macia)
 *          Guillermo Sabaté Castells (guillermo.sabate)
 **/

#ifndef _PROTOCOLMANAGER_H_
#define _PROTOCOLMANAGER_H_

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

#define DANNY "DANNY"
#define JACK "JACK"
#define ERROR "ERROR"
#define OK "CONNEXIO OK"


//STRUCTS

typedef struct{
  char source[14];
  char type;
  char data[100];
} Frame;

//FUNCTIONS

/**
 * 
 * 
 */
int protocolConnection(int sockfd, int sockfdclient);


#endif
